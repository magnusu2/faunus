#ifndef FAU_ISOBARIC_H
#define FAU_ISOBARIC_H

#include "faunus/moves/base.h"

namespace Faunus {
  /*!
   * \brief Fluctuate the volume against an external pressure 
   * \author Bjoern Persson and Mikael Lund
   * \todo Should take an arbritray vector of groups and implement a more efficent energy calculation.
   *       Salt scaling not yet implemented
   * \note Tested and found valid using debyehuckel_P3 and point charges and ideal systems. 
   * \note To compare with ideal systems sample the INVERSE of the volume to avoid (N+1)/N error! Intrinis property of the ensemble.
   * \date Lund/Canberra, Jan-Feb 2008
   *
   * This move is intended to maintain constant pressure in a canonical
   * Monte Carlo simulation. It will perform volume fluctuations and scale
   * particle positions according to the scaling algorithm specified for the container.
   */
  template<typename T> class isobaric : public markovmove {
  public:
    isobaric( ensemble&, container&, T &, double, int, int);
    string info();                          //!< Info string
    double move(vector<macromolecule> &);   //!< Markov move 
    average<double> vol, len, ilen, ivol;   //!< Averages
    void check(checkValue &);               //!< Unit test routine
    
  protected:
    T trialpot;                             //!< Copy of potential class for volume changes
    double P, dV, dh;                       //!< Pressure, volume difference and hamiltonian difference
    int maxlen, minlen;                     //!< Window parameters
    double newV;                            //!< New volume
    void newvolume();                       //!< Calculate new volume
    void accept();                          //!< Update volume and pair potential
    void updateVars();                      //!< Update averages and variables after an attempted move
    virtual double penaltyEnergy();         //!< Penalty energy (if any)
    T *pot;                                 //!< Override markovmove pointer.
  };
  
  /*!
   * \note If your pair-potential depends on the geometry of the 
   *       container (minimum image, for example) make sure that
   *       the pair-potential has a correct implementation of the
   *       setvolume() function. To allow for parallization this 
   *       class will instantiate a new (private) interaction class
   *       and in this way allow for two different container sizes.
   *
   * \param pressure bulk pressure in A^-3
   * \param i        Potential class. Make sure that the T_pairpot::setvolume() function is appropriate.
   * \param maxsize  Maximum allowed box length (A)
   * \param minsize  Minimum allowed box length (A)
   */
  template<typename T> isobaric<T>::isobaric
  ( ensemble &e, container &c, T &i, double pressure, int maxsize, int minsize )
  : markovmove(e,c,i), trialpot(i), minlen(minsize), maxlen(maxsize), P(pressure)  {
    name="ISOBARIC VOLUME MOVE";
    runfraction=1;
    dp=100; 
    pot=&i;
  }
  
  template<typename T> string isobaric<T>::info() {
    std::ostringstream o;
    o << markovmove::info();
    o << "#   Min/Max box length        = "<<minlen<<" / "<<maxlen<<endl
    <<   "#   External pressure         = "<< P <<"(A^-3) = "<<P*1660<<" (M)"<< endl
    <<   "#   Average volume            = "<< vol.avg() << " ("<<vol.stdev()<<") A^3" << endl
    <<   "#   Average box length        = "<< len.avg() << " ("<<len.stdev()<<") A^3" << endl;
    return o.str();
  }
  
  template<typename T> void isobaric<T>::check(checkValue &test) {
    markovmove::check(test);
    test.check("ISOBARIC_AvgBoxLen", len.avg() );
  }
  
  template<typename T> void isobaric<T>::newvolume() {
    newV = exp(log(con->getvolume())  // Random move in ln(V)
               + slp.random_half()*dp);
    trialpot.pair.setvolume(newV);    // Set volume for trial-pair-potential
    dV=newV - con->getvolume();
  }
  
  template<typename T> void isobaric<T>::accept() {
    naccept++;
    rc=OK;
    utot+=du;
    dpsqr+=pow(newV,1./3.);
    con->setvolume(newV);         // save the trial volume in the container
    pot->pair.setvolume(newV);    // ...AND in the pair-potential function
  }
  
  template<typename T> void isobaric<T>::updateVars() {
    double v=con->getvolume();
    double l=pow(v,1./3.);
    vol += v;
    ivol+= 1/v;
    len += l;
    ilen+= 1/l;
  }
  
  template<typename T> double isobaric<T>::penaltyEnergy() { return 0; }
  
  template<typename T> double isobaric<T>::move(vector<macromolecule> &g) {
    du=0;
    if (slp.runtest(runfraction)==false)
      return du;
    cnt++;
    double uold=0, unew=0;
    newvolume();                                 // Generate new trial volume - prep. trial potential
    double newL=pow(newV,1./3.);                 // New length
    if (newL>minlen && newL<maxlen) {            // Test for min/max box length
      int N=g.size();
      for (int i=0; i<N; i++)                    // Loop over macromolecules
        g[i].isobaricmove(*con, newL);           // ...and scale their mass-centers
#pragma omp parallel for reduction (+:uold,unew) schedule (dynamic)
      for (int j=0; j<N-1; j++) {
        for (int k=j+1; k<N; k++) {
          uold += pot->energy(con->p, g[j], g[k]);         // calc. old energy with original potential class
          unew += trialpot.energy(con->trial, g[j], g[k]); // calc. new energy using a COPY of the potential class
        }
      }
      du = unew-uold;
      dh = du + P*dV-(N+1)*log( newV/con->getvolume() );   // NPT energy
      dh += penaltyEnergy();                               // add penalty energy (if any)
      if (ens->metropolis(dh)==true ) {
        accept();
        updateVars();
        for (int i=0; i<N; i++)
          g[i].accept(*con);
        return du;
      } else rc=ENERGY;
      // reject because of energy:
      du=0;
      dpsqr+=0;
      for (int i=0; i<N; i++)
        g[i].undo(*con);
      updateVars();
      return du;
    }
    // reject because outside allowed min/max boxlen:
    dpsqr+=0;
    updateVars();
    return du;
  } 
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  
  /*!
   * \brief Fluctuate the volume against an external pressure (with penalty funtion)
   * \author Bjoern Persson and Mikael Lund
   */
  template<typename T> class isobaricPenalty : public isobaric<T> {
    using isobaric<T>::maxlen;
    using isobaric<T>::minlen;
    using isobaric<T>::newV;
    using isobaric<T>::con;
  private:
    virtual double penaltyEnergy();         // Penalty energy
    bool penalize;                          // Use penalty function
    int scale;
    vector<double> penalty;                 // Initial penalty function
    vector<double> newpenalty;              // New penalty function
    double pendiff();                       // Retruns the penalty potential diff of old and new volume
    int penbin();                           // Retruns the bin pointer of the present volume
    void updateVars();                      // Update averages and variables after an attempted move
    
  public:
    double pen;                             //!< Penalty to update penalty function
    double initpen;
    double scalepen;
    void loadpenaltyfunction(string);       //!< Load old penalty function 
    void printupdatedpenalty(string);       //!< Print penalty, old+new
    void printpenalty(string);              //!< Old penalty
    void updatepenalty();                   //!< Add penalty and newpenalty
    string info();
    isobaricPenalty( ensemble&, container&, T &, double, double, double, int, int, int);
  };
  
  template<typename T> isobaricPenalty<T>::isobaricPenalty
  ( ensemble &e, container &c, T &i,
   double pressure, double PEN, double scpen, int maxsize, int minsize, int sc ) : isobaric<T>(e,c,i,pressure,maxsize,minsize)
  {
    scale=sc;
    // Add new fuction to set penalty fuctions (minlen-maxlen)/scale
    int penbins= (maxlen-minlen)/scale;
    if ((maxlen-minlen)%scale!=0)
      penbins++; //number of bins, add one if range%scale!=0 
    penalty.clear();
    penalty.resize(penbins, 1.0e-20);
    pen=PEN;
    initpen=PEN;
    scalepen=scpen;
    newpenalty.clear();
    newpenalty.resize(penbins, 1.0e-20);
    penalize=false;
  }
  
  template<typename T> void isobaricPenalty<T>::printupdatedpenalty(string file) {
    std::ofstream f(file.c_str());
    if (f) {
      int j=penalty.size();
      f.precision(12);
      for (int i=0; i<j; i++) 
        f << (i+minlen)*scale << " " << penalty[i]+newpenalty[i]<<endl;
      f.close();
    }
  }
  
  template<typename T> void isobaricPenalty<T>::printpenalty(string file) {
    std::ofstream f(file.c_str());
    if (f) {
      int j=penalty.size();
      f.precision(12);
      for (int i=0; i<j; i++)
        f << (i+minlen)*scale << " " << penalty[i]<<endl;
      f.close();
    }
  }
  
  template<typename T> void isobaricPenalty<T>::loadpenaltyfunction(string file) {
    //Set up penalty functions based on min,max and scale and enables penalty function updating
    penalize=true;    //Boolean to determine update of newpenalty
    std::cout << "# Let's go BIASED!!!" << endl;
    int penbins= (maxlen-minlen)/scale;
    if ((maxlen-minlen)%scale!=0) penbins++; //number of bins, add one if range%scale!=0 
    penalty.clear();
    penalty.resize(penbins,1.0e-20);
    newpenalty.clear();
    newpenalty.resize(penbins,1.0e-20);
    vector<string> v;
    string s;
    v.clear();
    std::ifstream f(file.c_str() );
    if (f) {
      while (getline(f,s))
        v.push_back(s);
      f.close();
      std::cout << "# Penalty function loaded!!" << endl;
      short c=v.size();
      if (c==penbins) {
        double val;
        double bin;
        for (short i=0;i<c;i++) {
          val=0;
          bin=0;
          std::stringstream o;
          o << v[i];
          o >> bin >> val;
          penalty[(bin-minlen)/scale]=val;
        }
      } else {
        std::cout << "# Loaded penalty function does not fit range" << endl
        << "# Wrong min/maxlen or scale?" << endl;  
        std::ofstream fback("penaltybackup.dat");
        for (int i=0; i<v.size(); i++)
          fback <<v[i]<<endl;
        fback.close();
        std::cout << "# Backup of input penalty as penaltybackup.dat" << endl;
      }
    }
    else
      std::cout << "# WARNING! Penalty function " << file << " NOT READ!\n";
  }
  
  template<typename T> string isobaricPenalty<T>::info() {
    std::ostringstream o;
    o << isobaric<T>::info()
      << "#   Penalty function is:        " << ((penalize==true) ? "On" : "Off") << endl;
    if (penalize==true)
      o << "#   Penalty                   = "<< pen << " kT per observation" << endl
      <<   "#   Initial penalty           = "<< initpen << " kT per observation" << endl;
    return o.str();
  }
  
  template<typename T> double isobaricPenalty<T>::pendiff() {
    double penval;
    penval=penalty[int( pow(newV, 1./3.) - minlen)/scale]
    -penalty[int( pow(con->getvolume(),1./3.) - minlen)/scale];
    return penval;
  }
  
  template<typename T> int isobaricPenalty<T>::penbin() {
    int binnr;
    binnr=int(pow(con->getvolume(),1./3.)-minlen)/scale;
    return binnr;
  }
  
  template<typename T> void isobaricPenalty<T>::updatepenalty() {
    for (int i=0; i<penalty.size(); i++) {
      penalty[i]+=newpenalty[i];
      newpenalty[i]=0;
    }
    pen*=scalepen;
  }
  
  template<typename T> void isobaricPenalty<T>::updateVars() {
    isobaric<T>::updateVars();
    if (pen!=0.)
      newpenalty[penbin()]+=pen;
  }
  
  template<typename T> double isobaricPenalty<T>::penaltyEnergy() {
    if (penalize==true)
      return pendiff();
    else return 0;
  }
  
} // namespace
#endif
