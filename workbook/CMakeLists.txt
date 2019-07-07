# ======================================================================
#  art workbook main build file.
#
#  cd .../path/to/build/directory
#  cmake .../path/to/art-workbook
#  make [-jN]
# ======================================================================

cmake_minimum_required (VERSION 3.3)
project(art-workbook)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT WERROR NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-private-field)

cet_report_compiler_flags()

# Versions are specified in the ups/product_deps file
find_ups_product( toyExperiment )
find_ups_root()

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
