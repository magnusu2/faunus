// This file has been generated by Py++.

#include "boost/python.hpp"
#include "generated/generated_header.h"
#include "atoms.pypp.hpp"

namespace bp = boost::python;

void register_atoms_class(){

    { //::Faunus::atoms
        typedef bp::class_< Faunus::atoms > atoms_exposer_t;
        atoms_exposer_t atoms_exposer = atoms_exposer_t( "atoms", bp::init< >() );
        bp::scope atoms_scope( atoms_exposer );
        bp::class_< Faunus::atoms::data >( "data" )    
            .def( bp::self == bp::self )    
            .def_readwrite( "charge", &Faunus::atoms::data::charge )    
            .def_readwrite( "eps", &Faunus::atoms::data::eps )    
            .def_readwrite( "hydrophobic", &Faunus::atoms::data::hydrophobic )    
            .def_readwrite( "id", &Faunus::atoms::data::id )    
            .def_readwrite( "mw", &Faunus::atoms::data::mw )    
            .def_readwrite( "name", &Faunus::atoms::data::name )    
            .def_readwrite( "pka", &Faunus::atoms::data::pka )    
            .def_readwrite( "radius", &Faunus::atoms::data::radius )    
            .def_readwrite( "sigma", &Faunus::atoms::data::sigma );
        { //::Faunus::atoms::find
        
            typedef char ( ::Faunus::atoms::*find_function_type )( ::std::string ) ;
            
            atoms_exposer.def( 
                "find"
                , find_function_type( &::Faunus::atoms::find )
                , ( bp::arg("arg0") ) );
        
        }
        { //::Faunus::atoms::find
        
            typedef char ( ::Faunus::atoms::*find_function_type )( double,double ) ;
            
            atoms_exposer.def( 
                "find"
                , find_function_type( &::Faunus::atoms::find )
                , ( bp::arg("arg0"), bp::arg("arg1")=1.000000000000000055511151231257827021181583404541015625e-1 ) );
        
        }
        { //::Faunus::atoms::get
        
            typedef ::Faunus::particle ( ::Faunus::atoms::*get_function_type )( char ) ;
            
            atoms_exposer.def( 
                "get"
                , get_function_type( &::Faunus::atoms::get )
                , ( bp::arg("arg0") ) );
        
        }
        { //::Faunus::atoms::info
        
            typedef ::std::string ( ::Faunus::atoms::*info_function_type )(  ) ;
            
            atoms_exposer.def( 
                "info"
                , info_function_type( &::Faunus::atoms::info ) );
        
        }
        { //::Faunus::atoms::load
        
            typedef bool ( ::Faunus::atoms::*load_function_type )( ::std::string ) ;
            
            atoms_exposer.def( 
                "load"
                , load_function_type( &::Faunus::atoms::load )
                , ( bp::arg("arg0") ) );
        
        }
        { //::Faunus::atoms::load
        
            typedef bool ( ::Faunus::atoms::*load_function_type )( ::Faunus::inputfile & ) ;
            
            atoms_exposer.def( 
                "load"
                , load_function_type( &::Faunus::atoms::load )
                , ( bp::arg("arg0") ) );
        
        }
        { //::Faunus::atoms::operator()
        
            typedef ::Faunus::particle ( ::Faunus::atoms::*__call___function_type )( ::std::string ) ;
            
            atoms_exposer.def( 
                "__call__"
                , __call___function_type( &::Faunus::atoms::operator() )
                , ( bp::arg("arg0") ) );
        
        }
        { //::Faunus::atoms::operator()
        
            typedef ::Faunus::particle ( ::Faunus::atoms::*__call___function_type )( char ) ;
            
            atoms_exposer.def( 
                "__call__"
                , __call___function_type( &::Faunus::atoms::operator() )
                , ( bp::arg("arg0") ) );
        
        }
        { //::Faunus::atoms::operator[]
        
            typedef ::Faunus::atoms::data & ( ::Faunus::atoms::*__getitem___function_type )( ::std::string ) ;
            
            atoms_exposer.def( 
                "__getitem__"
                , __getitem___function_type( &::Faunus::atoms::operator[] )
                , ( bp::arg("arg0") )
                , bp::return_internal_reference< >() );
        
        }
        { //::Faunus::atoms::operator[]
        
            typedef ::Faunus::atoms::data & ( ::Faunus::atoms::*__getitem___function_type )( char ) ;
            
            atoms_exposer.def( 
                "__getitem__"
                , __getitem___function_type( &::Faunus::atoms::operator[] )
                , ( bp::arg("arg0") )
                , bp::return_internal_reference< >() );
        
        }
        { //::Faunus::atoms::reset_properties
        
            typedef void ( ::Faunus::atoms::*reset_properties_function_type )( ::std::vector< Faunus::particle > & ) ;
            
            atoms_exposer.def( 
                "reset_properties"
                , reset_properties_function_type( &::Faunus::atoms::reset_properties )
                , ( bp::arg("arg0") ) );
        
        }
        { //::Faunus::atoms::set
        
            typedef ::Faunus::particle ( ::Faunus::atoms::*set_function_type )( ::Faunus::particle &,char ) ;
            
            atoms_exposer.def( 
                "set"
                , set_function_type( &::Faunus::atoms::set )
                , ( bp::arg("arg0"), bp::arg("arg1") ) );
        
        }
        atoms_exposer.def_readwrite( "eps", &Faunus::atoms::eps );
        atoms_exposer.def_readwrite( "list", &Faunus::atoms::list );
        atoms_exposer.def_readwrite( "sigma", &Faunus::atoms::sigma );
    }

}
