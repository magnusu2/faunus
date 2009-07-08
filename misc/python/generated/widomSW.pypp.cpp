// This file has been generated by Py++.

#include "boost/python.hpp"
#include "generated/generated_header.h"
#include "widomSW.pypp.hpp"

namespace bp = boost::python;

struct widomSW_wrapper : Faunus::widomSW, bp::wrapper< Faunus::widomSW > {

    widomSW_wrapper(Faunus::widomSW const & arg )
    : Faunus::widomSW( arg )
      , bp::wrapper< Faunus::widomSW >(){
        // copy constructor
        
    }

    widomSW_wrapper(int arg0=10 )
    : Faunus::widomSW( arg0 )
      , bp::wrapper< Faunus::widomSW >(){
        // constructor
    
    }

    virtual ::std::string info(  ) {
        if( bp::override func_info = this->get_override( "info" ) )
            return func_info(  );
        else
            return this->Faunus::widomSW::info(  );
    }
    
    
    ::std::string default_info(  ) {
        return Faunus::widomSW::info( );
    }

};

void register_widomSW_class(){

    { //::Faunus::widomSW
        typedef bp::class_< widomSW_wrapper, bp::bases< Faunus::analysis > > widomSW_exposer_t;
        widomSW_exposer_t widomSW_exposer = widomSW_exposer_t( "widomSW", bp::init< bp::optional< int > >(( bp::arg("arg0")=(int)(10) )) );
        bp::scope widomSW_scope( widomSW_exposer );
        bp::implicitly_convertible< int, Faunus::widomSW >();
        { //::Faunus::widomSW::add
        
            typedef void ( ::Faunus::widomSW::*add_function_type )( ::Faunus::particle ) ;
            
            widomSW_exposer.def( 
                "add"
                , add_function_type( &::Faunus::widomSW::add )
                , ( bp::arg("arg0") ) );
        
        }
        { //::Faunus::widomSW::add
        
            typedef void ( ::Faunus::widomSW::*add_function_type )( ::Faunus::container & ) ;
            
            widomSW_exposer.def( 
                "add"
                , add_function_type( &::Faunus::widomSW::add )
                , ( bp::arg("arg0") ) );
        
        }
        { //::Faunus::widomSW::info
        
            typedef ::std::string ( ::Faunus::widomSW::*info_function_type )(  ) ;
            typedef ::std::string ( widomSW_wrapper::*default_info_function_type )(  ) ;
            
            widomSW_exposer.def( 
                "info"
                , info_function_type(&::Faunus::widomSW::info)
                , default_info_function_type(&widomSW_wrapper::default_info) );
        
        }
        { //::Faunus::widomSW::insert
        
            typedef void ( ::Faunus::widomSW::*insert_function_type )( ::Faunus::container &,::Faunus::energybase & ) ;
            
            widomSW_exposer.def( 
                "insert"
                , insert_function_type( &::Faunus::widomSW::insert )
                , ( bp::arg("arg0"), bp::arg("arg1") ) );
        
        }
    }

}