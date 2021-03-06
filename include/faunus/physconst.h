#ifndef FAU_PHYSCONST_H
#define FAU_PHYSCONST_H

#ifndef SWIG
#include <cmath>
#include <climits>
#endif

namespace Faunus {
  /**
   * @brief Physical constants and parameters.
   *
   * This class stores physical constants, temperature
   * as well as various conversion functions. All data
   * and functions are `static` and can thus be called
   * anywhere without construction.
   */
  template<class Td>
    class PhysicalConstants {
      private:
        static Td _T;          //!< Temperature [K]
      public:
        PhysicalConstants(Td=298.15);
        static const Td
          pi,                  //!< The number pi
          infty,               //!< Infinity
          e0,                  //!< Permittivity of vacuum [C^2/(J*m)]
          kB,                  //!< Boltzmann's constant [J/K]
          e,                   //!< Absolute electronic unit charge [C] 
          R,                   //!< Molar gas constant [J/(K*mol)]
          Nav;                 //!< Avogadro's number [1/mol]
        static Td lB(Td);      //!< Bjerrum length [Aangstrom]
        static Td kT2kJ(Td=1); //!< kT/molecule -> kJ/mol
        static Td kJ2kT(Td=1); //!< kJ/mol -> kT/molecule
        static Td T();         //!< Return temperature [K]
        static void setT(Td);  //!< Set temperature [K]
        static Td D2eA(Td=1);  //!< Debye to electron Angstrom
    };


#ifdef __INTEL_COMPILER
  // needed due to constexpr bug in intel13 compiler. Fixed?
  template<class Td>
    const Td PhysicalConstants<Td>::infty=-std::log(0.);
#else
  template<class Td>
    const Td PhysicalConstants<Td>::infty=std::numeric_limits<Td>::infinity();
#endif

  template<class Td>
    const Td PhysicalConstants<Td>::pi=std::acos(-1.);

  template<class Td>
    const Td PhysicalConstants<Td>::e0=8.85419e-12;

  template<class Td>
    const Td PhysicalConstants<Td>::e=1.602177e-19;

  template<class Td>
    const Td PhysicalConstants<Td>::kB=1.380658e-23;

  template<class Td>
    const Td PhysicalConstants<Td>::Nav=6.022137e23;

  template<class Td>
    const Td PhysicalConstants<Td>::R=kB*Nav;

  template<class Td>
    Td PhysicalConstants<Td>::_T=298.15;

  template<class Td>
    PhysicalConstants<Td>::PhysicalConstants(Td temp) { setT(temp); }

  template<class Td>
    void PhysicalConstants<Td>::setT(Td temp) { _T=temp; }

  template<class Td>
    Td PhysicalConstants<Td>::T() { return _T; }

  template<class Td>
    Td PhysicalConstants<Td>::lB(Td e_r) {
      return e*e / (4*pi*e0*e_r*1e-10*kB*_T);
    }

  template<class Td>
    Td PhysicalConstants<Td>::kT2kJ(Td u) { return u*kB*_T*Nav*1e-3; }

  template<class Td>
    Td PhysicalConstants<Td>::kJ2kT(Td u) { return u/kT2kJ(1); }

  template<class Td>
    Td PhysicalConstants<Td>::D2eA(Td D) { return 0.20819434*D; }

  typedef PhysicalConstants<double> pc;      //!< Typedef for PhysicalConstants

}// namespace
#endif


