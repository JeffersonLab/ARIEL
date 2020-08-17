# ======================================================================
#  toy experiment main build file.
#
#  cd .../path/to/build/directory
#  source .../path/to/art_hough/ups/setup_for_development
#  buildtool -f[T]l
# ======================================================================

cmake_minimum_required (VERSION 3.12...3.14 FATAL_ERROR)
project(toyexperiment LANGUAGES CXX)

# cetbuildtools contains our cmake modules
SET(CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION})
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH $ENV{ART_DIR}/Modules
                      $ENV{CANVAS_IO_DIR}/Modules
                      $ENV{CETBUILDTOOLS_DIR}/Modules
                      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic)

cet_report_compiler_flags()

check_ups_version(art "$ENV{ART_VERSION}" v3_02_00 PRODUCT_OLDER_VAR OLDER_THAN_v30200)

# Versions are specified in the ups/product_deps file

find_ups_product(art)
find_ups_product(canvas_root_io)
find_ups_product(messagefacility)
find_ups_product(clhep)
find_ups_product(cetbuildtools)
find_ups_root()
find_ups_boost()

if (NOT OLDER_THAN_v30200)
  find_ups_product(art_root_io)
  add_definitions(-DART_ROOT_IO)
endif()

message(STATUS "Project is ${product} ${version} ${qualifier}")

# macros for artdaq_dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# testing
#add_subdirectory(test)

# source
add_subdirectory(toyExperiment)

# Scripts
add_subdirectory(scripts)

# Admin stuff
add_subdirectory(admin)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
