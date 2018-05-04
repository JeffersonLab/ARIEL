#
#  CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for project MessageFacility
#
# Create a build directory and an install direcotory which are outside the source code directory
#  cd .../path/to/build/directory
#  source .../path/to/source/code/setup_for_development
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)

cmake_minimum_required (VERSION 2.8) # use cmake 2.8 or later

project(messagefacility)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor
  )
cet_report_compiler_flags()


# these are minimum required versions, not the actual product versions
# find_ups_product defines variables for external libraries and executables
find_ups_product(cetbuildtools v4_14_01)
find_ups_product(cetlib_except v1_00_00)
find_ups_product(cetlib v2_00_00)
find_ups_product(fhiclcpp v2_17_00)
find_ups_product(sqlite v3_08_05_00 )
find_ups_product(hep_concurrency)
find_ups_boost(v1_50_0)
find_ups_product(tbb)

# SQLite
cet_find_library(SQLITE3 NAMES sqlite3_ups PATHS ENV SQLITE_LIB NO_DEFAULT_PATH)

# The environment has been established, now generate the Makefiles.

add_subdirectory(messagefacility)
add_subdirectory(perllib)
add_subdirectory(xcompile)
add_subdirectory(ups)

# This step enables building the package tarball
# packaging utility
include(UseCPack)
