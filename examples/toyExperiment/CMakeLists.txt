# ======================================================================
#  toy experiment main build file.
# ======================================================================

cmake_minimum_required (VERSION 3.12...3.14 FATAL_ERROR)
project(toyexperiment LANGUAGES CXX)

find_package(cetbuildtools REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic)
cet_report_compiler_flags()

# Versions are specified in the ups/product_deps file
find_ups_product(art v3_00_00)
find_ups_product(canvas_root_io)
find_ups_product(messagefacility)
find_ups_product(clhep)
find_ups_root()
find_ups_boost(BOOST_TARGETS)

# define CLHEP
cet_find_library(CLHEP  NAMES CLHEP   PATHS ENV CLHEP_LIB_DIR NO_DEFAULT_PATH)

check_ups_version(art "${art_UPS_VERSION}" v3_02_00 PRODUCT_OLDER_VAR OLDER_THAN_v30200)
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
