// This file has been generated by Py++.

#include "boost/python.hpp"
#include "generated/generated_header.h"
#include "ioaam.pypp.hpp"

namespace bp = boost::python;

struct ioaam_wrapper : Faunus::ioaam, bp::wrapper< Faunus::ioaam > {

    ioaam_wrapper(Faunus::ioaam const & arg )
    : Faunus::ioaam( arg )
      , bp::wrapper< Faunus::ioaam >(){
        // copy constructor
        
    }

    ioaam_wrapper( )
    : Faunus::ioaam( )
      , bp::wrapper< Faunus::ioaam >(){
        // null constructor
    
    }

    virtual ::std::vector< Faunus::particle > load( ::std::string arg0 ) {
        if( bp::override func_load = this->get_override( "load" ) )
            return func_load( arg0 );
        else
            return this->Faunus::ioaam::load( arg0 );
    }
    
    
    ::std::vector< Faunus::particle > default_load( ::std::string arg0 ) {
        return Faunus::ioaam::load( arg0 );
    }

    virtual bool save( ::std::string arg0, ::std::vector< Faunus::particle > & arg1 ) {
        if( bp::override func_save = this->get_override( "save" ) )
            return func_save( arg0, boost::ref(arg1) );
        else
            return this->Faunus::ioaam::save( arg0, boost::ref(arg1) );
    }
    
    
    bool default_save( ::std::string arg0, ::std::vector< Faunus::particle > & arg1 ) {
        return Faunus::ioaam::save( arg0, boost::ref(arg1) );
    }

};

void register_ioaam_class(){

    bp::class_< ioaam_wrapper >( "ioaam", bp::init< >() )    
        .def( 
            "load"
            , (::std::vector< Faunus::particle > ( ::Faunus::ioaam::* )( ::std::string ) )(&::Faunus::ioaam::load)
            , (::std::vector< Faunus::particle > ( ioaam_wrapper::* )( ::std::string ) )(&ioaam_wrapper::default_load)
            , ( bp::arg("arg0") ) )    
        .def( 
            "load"
            , (void ( ::Faunus::ioaam::* )( ::Faunus::container &,::Faunus::inputfile &,::std::vector< Faunus::macromolecule > & ) )( &::Faunus::ioaam::load )
            , ( bp::arg("arg0"), bp::arg("arg1"), bp::arg("arg2") ) )    
        .def( 
            "load"
            , (bool ( ::Faunus::ioaam::* )( ::Faunus::container &,::std::string ) )( &::Faunus::ioaam::load )
            , ( bp::arg("arg0"), bp::arg("arg1") ) )    
        .def( 
            "loadlattice"
            , (void ( ::Faunus::ioaam::* )( ::Faunus::container &,::Faunus::inputfile &,::std::vector< Faunus::macromolecule > & ) )( &::Faunus::ioaam::loadlattice )
            , ( bp::arg("arg0"), bp::arg("arg1"), bp::arg("arg2") ) )    
        .def( 
            "save"
            , (bool ( ::Faunus::ioaam::* )( ::std::string,::std::vector< Faunus::particle > & ) )(&::Faunus::ioaam::save)
            , (bool ( ioaam_wrapper::* )( ::std::string,::std::vector< Faunus::particle > & ) )(&ioaam_wrapper::default_save)
            , ( bp::arg("arg0"), bp::arg("arg1") ) );

}
