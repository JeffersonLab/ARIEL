# ======================================================================
#  ToyCmake main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/ToyCmake/ups/setup_for_development <-d|-o|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package
# ======================================================================


# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(ToyCmake)

# cetbuildtools contains our cmake modules
set( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
if( NOT CETBUILDTOOLS_VERSION )
  message( FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules" )
endif()
set(CMAKE_MODULE_PATH $ENV{CETBUILDTOOLS_DIR}/Modules 
		      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT WERROR EXTRA_FLAGS -pedantic)
cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
#find_ups_product( cppunit v1_12_1 )
#find_ups_product( gccxml v0_9 )
#find_ups_product( libsigcpp v2_2_10 )
#find_ups_product( clhep v2_1_1_0 )

find_ups_product( cetbuildtools v2_03_00 )
#find_ups_product( cpp0x v1_03_00 )
#find_ups_product( cetlib v1_03_00 )
#find_ups_product( fhiclcpp v2_16_00 )
#find_ups_product( messagefacility v1_10_00 )
#find_ups_product( sqlite v3_07_08_00 )
#find_ups_root( v5_30_02 )
#find_ups_boost( v1_47_0 filesystem
#                        program_options
#                        regex
#                        thread
#                        unit_test_framework
# )

cet_check_gcc()

# include search path
# notice use of ENV to invoke an external variable
#include_directories ( $ENV{LIBSIGCPP_INC}/sigc++-2.0 )
#include_directories ( $ENV{LIBSIGCPP_LIB}/sigc++-2.0/include )

# define CLHEP
#find_library( CLHEP  NAMES CLHEP   PATHS $ENV{CLHEP_BASE}/lib NO_DEFAULT_PATH )

# define CETLIB
#find_library( CETLIB NAMES cetlib PATHS $ENV{CETLIB_LIB} NO_DEFAULT_PATH )

# define FHICLCPP
#find_library( FHICLCPP NAMES fhiclcpp PATHS $ENV{FHICLCPP_LIB} NO_DEFAULT_PATH )

# have to make sure cmake doesn't select the system libraries
# define libsigc
#find_library( SIGC    NAMES sigc-2.0  PATHS $ENV{LIBSIGCPP_LIB} NO_DEFAULT_PATH )
# define cppunit
#find_library( CPPUNIT NAMES cppunit   PATHS $ENV{CPPUNIT_LIB}   NO_DEFAULT_PATH )

#find_library(SQLITE3 NAMES sqlite3 PATHS $ENV{SQLITE_LIB} NO_DEFAULT_PATH)

# source
add_subdirectory (ToyCmake)

# testing
add_subdirectory(test)

# ups - table and config files
add_subdirectory(ups)

# packaging utility - enable building a package tarball
include(UseCPack)
