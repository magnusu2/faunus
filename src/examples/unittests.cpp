#undef NDEBUG
#define CATCH_CONFIG_MAIN  // This tell CATCH to provide a main() - only do this in one cpp file
#include <catch/catch.hpp>
#include <faunus/faunus.h>

using namespace Faunus;

/* check tabulator */
template<typename Ttabulator>
void checkTabulator(Ttabulator t) {
  double tol=0.01;
  t.setRange(0.9, 100);
  t.setTolerance(tol,tol);
  std::function<double(double)> f = [](double x) { return 1/x; };
  auto data = t.generate(f); 
  for (double x=1.0; x<100; x+=1) {
    double error = fabs( f(x) - t.eval(data,x) );
    CHECK(error>0);
    CHECK(error<tol);
  }
}

TEST_CASE("Spline table", "...")
{
  INFO("hej");
  checkTabulator(Tabulate::Hermite<double>());
  checkTabulator(Tabulate::AndreaIntel<double>());
  checkTabulator(Tabulate::Andrea<double>());
  checkTabulator(Tabulate::Linear<double>());

  PointParticle a,b;
  a.charge=1;
  b.charge=-1;
  a.radius=b.radius=2;
  InputMap mcp;
  Potential::DebyeHuckelLJ pot_org(mcp);
  Potential::PotentialTabulate<Potential::DebyeHuckelLJ> pot_tab(mcp);

  double error = fabs( pot_org(a,b,5)-pot_tab(a,b,5) ) ;
  CHECK(error>0);
  CHECK(error<0.01);
}

/*
 * Check various copying operations
 * between particle types
 */
template<typename Tparticle>
void checkParticle() {
  Tparticle p;
  p.clear();
  p.mw=1.0; // set some property - should not be overridden!
  Point::Tvec vec(1,0,0);
  p=vec;
  CHECK( p.norm() == Approx(1) );
  p+=vec;
  CHECK( p.norm() == Approx(2) );
  p*=0.5;
  CHECK( p.norm() == Approx(1) );

  CHECK( p.mw == Approx(1) ); // mw should not be overridden!

  p=Point::Tvec(2,3,5);
  CHECK( p.len() == Approx( sqrt(4+9+25) ) );

  CHECK( p.mw == Approx(1) ); // mw should not be overridden!

  PointParticle a;
  a.clear();
  a.x()=10.;
  a.charge=1.0;
  p=a; 
  CHECK( p.x() == Approx(a.x()) );
  CHECK( p.charge == Approx(a.charge) );
  CHECK( p.mw == Approx(0) );
}

TEST_CASE("Particles", "...")
{
  checkParticle<PointParticle>();
  checkParticle<DipoleParticle>();
  checkParticle<CigarParticle>();
}

TEST_CASE("Groups", "Check group range and size properties")
{
  Group g(2,5);           // first, last particle

  CHECK(g.front()==2);
  CHECK(g.back()==5);
  CHECK(g.size()==4);

  g.resize(0);
  CHECK(g.empty());

  g.resize(1000);
  CHECK(g.size()==1000);

  g.setfront(1);
  g.setback(10);
  CHECK(g.front()==1);
  CHECK(g.back()==10);
  CHECK(g.size()==10);

  int cnt=0;
  for (auto i : g) {
    cnt++;
    CHECK(g.find(i));
  }
  CHECK(cnt==g.size());
  CHECK(!g.find(0));
  CHECK(!g.find(11));

  // check random
  int min=1e6, max=-1e6;
  for (int n=0; n<1e3; n++) {
    int i=g.random();
    if (i<min) min=i;
    if (i>max) max=i;
    CHECK( g.find(i) );
  }
  CHECK(min==g.front());
  CHECK(max==g.back());
}

TEST_CASE("Math", "Checks mathematical functions")
{
  CHECK( pc::infty == -std::log(0) );
  CHECK( exp_cawley(1) == Approx(std::exp(1)).epsilon(0.05) );
  CHECK( invsqrtQuake(20.) == Approx(1/std::sqrt(20.)).epsilon(0.05) );
}

TEST_CASE("Geometries", "Geometry tests")
{
  Geometry::Sphere geoSph(1000);
  Geometry::Cylinder geoCyl(1000,1000);
  Point a(0,0,sqrt(16)), b(0,sqrt(64),0);
  double x = geoSph.sqdist(a,b);
  double y = geoCyl.sqdist(a,b);
  CHECK( x==Approx(16+64) );
  CHECK( x==Approx(y) );
}

TEST_CASE("Random numbers", "Check random number generator")
{
  int min=10, max=0, N=1e7;
  double x=0;
  for (int i=0; i<N; i++) {
    int j = slp_global.rand() % 10;
    if (j<min) min=j;
    if (j>max) max=j;
    x+=j;
  }
  CHECK( min==0 );
  CHECK( max==9 );
  CHECK( std::fabs(x/N) == Approx(4.5).epsilon(0.05) );
}

TEST_CASE("Quaternion", "Check vector rotation")
{
  Geometry::QuaternionRotate qrot;
  Geometry::Cylinder geo(1000,1000);
  Point a(0,0,0);
  a.x()=1.;
  qrot.setAxis( geo, Point(0,0,0), Point(0,1,0), pc::pi/2); // rotate around y-axis
  a = qrot(a); // rot. 90 deg.
  CHECK( a.x() == Approx(0.0) );
  a = qrot(a); // rot. 90 deg.
  CHECK( a.x() == Approx(-1.0) );
}

TEST_CASE("Tables and averages","Check table of averages")
{
  typedef Analysis::Table2D<float,Average<float> > Ttable;
  Ttable table(0.1, Ttable::XYDATA);
  table(2.1)+=1;
  table(2.1)+=3;
  CHECK( table(2.1).avg() == Approx(2.0) );
}
