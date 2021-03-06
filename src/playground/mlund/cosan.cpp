#include <faunus/faunus.h>
#include <tclap/CmdLine.h>

using namespace Faunus;
using namespace TCLAP;

typedef Geometry::Cuboid Tgeometry;
typedef Potential::CoulombSR<Tgeometry, Potential::Coulomb, Potential::LennardJones> Tpairpot;

int main(int argc, char** argv) {
  string inputfile,istate,ostate;
  try {
    cout << textio::splash();
    CmdLine cmd("Monte Carlo simulation of rigid molecules explicit salt", ' ', "0.1");
    ValueArg<string> inputArg("i","inputfile","InputMap key/value file",true,"cosan.input","inputfile");
    ValueArg<string> istateArg("c","instate","Name of input statefile",false,"state","instate");
    ValueArg<string> ostateArg("o","outstate","Name of output statefile",false,"state","outstate");
    cmd.add( inputArg );
    cmd.add( istateArg );
    cmd.add( ostateArg );
    cmd.parse( argc, argv );
    inputfile = inputArg.getValue();
    istate = istateArg.getValue();
    ostate = ostateArg.getValue();
  }
  catch (ArgException &e)  {
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
  }

  InputMap mcp(inputfile);
  MCLoop loop(mcp);                    // class for handling mc loops
  FormatPQR pqr;                       // PQR structure file I/O
  FormatAAM aam;                       // AAM structure file I/O
  FormatTopology top;
  FormatXTC xtc(1000);                 // XTC gromacs trajectory format
  EnergyDrift sys;                     // class for tracking system energy drifts
  UnitTest test(mcp);

  Energy::Hamiltonian pot;
  auto nonbonded = pot.create( Energy::Nonbonded<Tpairpot>(mcp) );
  Space spc( pot.getGeometry() );

  // Add polymers
  vector<GroupMolecular> pol( mcp.get("polymer_N",0));
  string polyfile = mcp.get<string>("polymer_file", "");
  for (auto &g : pol) {
    aam.load(polyfile);
    g = spc.insert( aam.p,-1,Space::OVERLAP );
    g.name="Protein";
    spc.enroll(g);
  }

  // Add salt
  GroupAtomic salt(spc, mcp);
  salt.name="Salt";
  spc.enroll(salt);
  spc.load(istate, Space::RESIZE);

  Analysis::RadialDistribution<float,int> rdf(0.2);
  rdf.maxdist=pow( spc.geo->getVolume(), 1/3.)/2;   // sample half box length

  Move::TranslateRotate gmv(mcp,pot,spc);
  Move::GrandCanonicalSalt gc(mcp,pot,spc,salt);
  Move::AtomicTranslation mv(mcp, pot, spc);
  mv.setGroup(salt);

  double utot=pot.external() + pot.g_internal(spc.p, salt);
  for (auto &p : pol)
    utot+=pot.g2g(spc.p, salt, p);
  for (auto i=pol.begin(); i!=pol.end()-1; i++)
    for (auto j=i+1; j!=pol.end(); j++)
      utot += pot.g2g(spc.p, *i, *j);
  sys.init( utot );

  cout << atom.info() << spc.info() << pot.info() << textio::header("MC Simulation Begins!");

  while ( loop.macroCnt() ) {  // Markov chain 
    while ( loop.microCnt() ) {
      int k,i=rand() % 3;
      switch (i) {
        case 0:
          mv.setGroup(salt);
          sys+=mv.move( salt.size()/2 );
          break;
        case 1:
          k=pol.size();
          while (k-->0) {
            gmv.setGroup( pol[ rand() % pol.size() ] );
            sys+=gmv.move();
          }
          for (auto i=pol.begin(); i!=pol.end()-1; i++)
            for (auto j=i+1; j!=pol.end(); j++)
              rdf( spc.geo->dist(i->cm,j->cm) )++;
          break;
        case 2:
          sys+=gc.move( salt.size()/2 );
          break;
      }
    } // end of micro loop

    double utot=pot.external() + pot.g_internal(spc.p, salt);
    for (auto &p : pol)
      utot+=pot.g2g(spc.p, salt, p);
    for (auto i=pol.begin(); i!=pol.end()-1; i++)
      for (auto j=i+1; j!=pol.end(); j++)
        utot += pot.g2g(spc.p, *i, *j);
    sys.checkDrift( utot );
    rdf.save("rdf_p2p.dat");
    cout << loop.timing();
  } // end of macro loop

  pqr.save("confout.pqr", spc.p);
  spc.save(ostate);

  cout << loop.info() << spc.info() << sys.info() << mv.info() << gmv.info() << gc.info();
}
