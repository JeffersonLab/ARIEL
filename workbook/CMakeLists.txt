# ======================================================================
#  art workbook main build file.
#
#  cd .../path/to/build/directory
#  source .../path/to/art_hough/ups/setup_for_development
#  buildtool -f[T]l
# ======================================================================

# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH
          $ENV{CANVAS_DIR}/Modules
		      $ENV{CETBUILDTOOLS_DIR}/Modules
		      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic)

cet_report_compiler_flags()

# Versions are specified in the ups/product_deps file
find_ups_product( toyExperiment )
find_ups_product( cetbuildtools )
find_ups_root()

message(STATUS "Project is ${product} ${version} ${qualifier}")

# Set cmake policy to turn off CMake warnings when working with older
# versions of art.
check_ups_version(art "$ENV{ART_VERSION}" v1_11_00 PRODUCT_OLDER_VAR have_old_art)
if (have_old_art)
  cmake_policy(SET CMP0046 OLD)
endif()
unset(have_old_art)

# macros for artdaq_dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# testing
#add_subdirectory(test)

# source
add_subdirectory(art-workbook)

# Admin stuff
add_subdirectory(admin)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
