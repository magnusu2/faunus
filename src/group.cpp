#include <faunus/geometry.h>
#include <faunus/point.h>
#include <faunus/species.h>
#include <faunus/physconst.h>
#include <faunus/group.h>
#include <faunus/average.h>
#include <faunus/space.h>
#include <faunus/inputfile.h>
#include <faunus/textio.h>

namespace Faunus {

  Group::Group(int front, int back) : Range(front,back-front+1) {
    if (front<0 || back<0)
      resize(0);
    w=15;
  }

  Group::~Group() {}

  double Group::charge(const p_vec &p) const {
    double z=0;
    for (auto i : *this)
      z+=p[i].charge;
    return z;
  }

  bool Group::operator==(const Group& g) const {
    return (*this == g);
  }

  std::ostream& Group::write(std::ostream &o) const {
    o << front() << " " << back() << " " << cm;
    return o;
  }

  std::ostream& operator<<(std::ostream &o, Group &g) {
    return g.write(o);
  }

  Group& Group::operator<<(std::istream &in) {
    int front;
    int back;
    in >> front >> back;
    setrange(front,back);
    assert( size()==back-front+1 && "Problem with Group range");
    cm.operator<<(in);
    return *this;
  }

  string Group::_info() {
    std::ostringstream o;
    return o.str();
  }

  string Group::info() {
    using namespace textio;
    std::ostringstream o;
    o << header("Group: " + name)
      << pad(SUB,w,"Size") << size() << endl;
    if (!empty())
      o << pad(SUB,w,"Range") << "[" << front() << "-" << back() << "]" << endl;
    o << pad(SUB,w,"Mass center") << cm.x() << " " << cm.y() << " " << cm.z() << endl;
    return o.str() + _info();
  }

  /*!
   * \warning Intra-molecular distances must not exceed half the box size for cubouid geometry.
   * \todo Implement assertion to catch failure when molecule is bigger than half the box size.
   */
  Point Group::_massCenter(const Space &spc) const {
    return Geometry::massCenter(*spc.geo, spc.p, *this);
  }

  Point Group::setMassCenter(const Space &spc) {
    cm=massCenter(spc);
    cm_trial=cm;
    return cm;
  }

  Point Group::massCenter(const Space &spc) const {
    assert(&spc!=nullptr);
    return _massCenter(spc);
  }

  Point Group::dipolemoment(const Space &s) const {
    Point t, mu(0,0,0);
    for (auto i : *this) {
      t=s.p[i] - cm;
      s.geo->boundary(t);
      mu += t*s.p[i].charge;
    }
    return mu;
  }

  /**
   * @param spc Simulation space
   * @param endpoint End point of rotation axis, starting from the mass center
   * @param angle [rad]
   */
  void Group::rotate(Space &spc, const Point &endpoint, double angle) {
    Geometry::QuaternionRotate vrot;
    assert( spc.geo->dist(cm,massCenter(spc) )<1e-6 );      // debug. Is mass center in sync?
    cm_trial = cm;
    vrot.setAxis(*spc.geo, cm, endpoint, angle);            // rotate around line between mass center and point
    for (auto i : *this)
      spc.trial[i].rotate(vrot);
    assert( spc.geo->dist(cm_trial, massCenter(spc))<1e-9 && "Rotation messed up mass center. Is the box too small?");
  }

  void Group::scale(Space &s, double newvol) {
    if (!empty()) {
      cm_trial=cm;
      cm_trial.scale(*s.geo, newvol);
      for (auto i : *this)
        s.trial[i].scale(*s.geo, newvol);
    }
  }

  void Group::undo(Space &s) {
    for (auto i : *this)
      s.trial[i]=s.p[i];
    cm_trial=cm;
  }

  void Group::accept(Space &s) {
    for (auto i : *this)
      s.p[i] = s.trial[i];
    cm=cm_trial;
  }

  /*!
   * \param spc Simulation Space
   * \param p Vector to translate with
   */
  void Group::translate(Space &spc, const Point &p) {
    assert( spc.geo->sqdist(cm,massCenter(spc))<1e-6  && "Mass center out of sync.");
    for (auto i : *this)
      spc.trial[i].translate(*spc.geo, p);
    cm_trial.translate(*spc.geo, p);
  }

  int Group::random() const {
    if (!empty()) {
      int i = front() + slp_global.rand() % size();
      assert(find(i) && "Generated random element out of range!");
      return i;
    }
    return -1;
  }

  bool Group::isAtomic() const { return false; }

  bool Group::isMolecular() const { return false; }

  int Group::numMolecules() const {
    return size();
  }

  GroupAtomic::GroupAtomic(int front, int back) : Group(front, back) {
  }

  GroupAtomic::GroupAtomic(Space &spc, InputMap &in) {
    add(spc,in);
  }

  bool GroupAtomic::isAtomic() const { return true; }

  /**
   * The InputMap is scanned for the following keywords, starting with X=1:
   *
   * Key            | Description
   * :------------- | :---------------------
   * `tionX`        | Name of atom X
   * `nionX`        | Number of type X atoms
   * `dpionX`       | (Displacement parameter of atom type X)
   * `aionX`        | (Activity of atom X (molar scale))
   *
   * If the two latter properties, displacement and activity, are omitted
   * (recommended) the values from AtomTypes is used instead. That is, you
   * should specify these directly in the input JSON file.
   *
   */
  void GroupAtomic::add(Space &spc, InputMap &in) {
    setfront( spc.p.size() );
    int size=0;
    int n=1, npart;
    do {
      std::ostringstream nion("nion"), tion("tion"), dpion("dpion"), aion("aion");
      nion << "nion" << n;
      tion << "tion" << n;
      dpion<< "dpion"<< n;
      aion << "aion" << n++; //activity
      npart = in.get(nion.str(), 0);
      if (npart>0) {
        short id = atom[ in.get<string>(tion.str(), "UNK") ].id;
        atom[id].dp = in.get(dpion.str(), atom[id].dp);
        atom[id].activity = in.get(aion.str(), 0.);
        spc.insert( atom[id].name, npart);
        size+=npart;
      } else break;
    } while (npart>0);
    if (size>0)
      resize(size);
    else
      resize(0);
    setMassCenter(spc);
    spc.enroll(*this);
  }

  GroupMolecular::GroupMolecular(int front, int back) : Group(front, back) {
  }

  bool GroupMolecular::isMolecular() const { return true; }

  int GroupMolecular::numMolecules() const { return 1; }

  void GroupMolecular::scale(Space &s, double newvol) {
    assert( s.geo->dist(cm, massCenter(s))<1e-6);
    assert( s.geo->dist(cm, cm_trial)<1e-7);

    Point newcm=cm;
    newcm.scale(*s.geo, newvol);
    translate(s,-cm);                 // move to origo

    double oldvol=s.geo->getVolume(); // store original volume
    s.geo->setVolume(newvol);         // apply trial volume

    for (auto i : *this) {
      s.trial[i] += newcm;            // move all particles to new cm
      s.geo->boundary( s.trial[i] );  // respect boundary conditions
    }
    cm_trial=newcm;
    s.geo->setVolume(oldvol);         // restore original volume
  }

  string GroupMolecular::_info() {
    using namespace textio;
    std::ostringstream o;
    return o.str();
  }

  GroupArray::GroupArray(int AtomsPerMolecule) : N(AtomsPerMolecule) {
    assert(N>0 && "Molecular size must be larger than zero");
  }

  int GroupArray::randomMol() const {
    int i=(random()-front())/N;
    assert(find(i) && "Out of range!");
    return i;
  }

  int GroupArray::numMolecules() const {
    assert( (size()%N)==0 );
    return size()/N;
  }

  /**
   * @warning You must manually update the mass center of the returned group
   */
  GroupMolecular& GroupArray::operator[](int i) {
    sel.setfront( front()+i*N );
    sel.setback( sel.front()+N-1 );
    assert( sel.size()==N );
    assert( find( sel.front() ) );
    assert( find( sel.back()  ) );
    return sel;
  }

  string GroupArray::_info() {
    using namespace textio;
    std::ostringstream o;
    o << pad(SUB,w,"Mol size") << N << endl
      << pad(SUB,w,"Molecules") << numMolecules() << endl;
    return o.str();
  }

  void GroupArray::add(const GroupMolecular &g) {
    if ((g.size()%N)==0) {
      if (empty())
        setrange(g.front(), g.back());
      else if (g.front()==back()+1)
        setback(g.back());
    }
    assert( (size()%N)==0 && "GroupArray not a multiple of N");
  }

  void GroupArray::scale(Space &s, double newvol) {
    for (int i=0; i<numMolecules(); i++) {
      operator[](i);
      sel.setMassCenter(s);
      sel.scale(s,newvol);
    }
  }

}//namespace
