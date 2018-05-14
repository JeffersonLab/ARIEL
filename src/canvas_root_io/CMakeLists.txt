# ======================================================================
#  art main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/art/ups/setup_for_development
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(canvas)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor
  )
cet_have_qual(sse2 SSE2)
if ( SSE2 )
  cet_add_compiler_flags(CXX -msse2 -ftree-vectorizer-verbose=2)
endif()

cet_report_compiler_flags()

find_ups_product(canvas)
find_ups_product(clhep)
find_ups_product(cetlib_except)
find_ups_product(cetlib v2_01_00)
find_ups_product(fhiclcpp)
find_ups_product(messagefacility)
find_ups_product(tbb)
find_ups_root()
find_ups_boost()
find_ups_product(cetbuildtools v7_00_00)


# define CLHEP
cet_find_library( CLHEP  NAMES CLHEP   PATHS ENV CLHEP_LIB_DIR NO_DEFAULT_PATH )

# art_dictionary
include(ArtDictionary)

# source
add_subdirectory(canvas_root_io)

# ups - table and config files
add_subdirectory(ups)

# CMake modules
add_subdirectory(Modules)

# tools - e.g. migration scripts
add_subdirectory(tools)

# packaging utility
include(UseCPack)
