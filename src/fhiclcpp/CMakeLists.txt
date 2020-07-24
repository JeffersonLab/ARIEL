#
#  fhiclcpp/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library fhiclcpp and fhiclcppD
#
#  cd .../path/to/build/directory
#  source .../path/to/source/code/setup_for_development
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)

cmake_minimum_required( VERSION 2.8 )  # use cmake 2.8 or later

project(fhiclcpp)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -Wno-error=register
  )
cet_report_compiler_flags()


# these are minimum required versions, not the actual product versions
find_ups_product(cetbuildtools v4_14_01)
find_ups_product(cetlib_except v1_00_00)
find_ups_product(cetlib v2_01_00)
find_ups_product(python)
find_ups_product(sqlite v3_07_15_00)
find_ups_boost(v1_50_0)

# Python
cet_find_library(PYTHON NAMES python2.7 PATHS ENV PYTHON_LIB NO_DEFAULT_PATH)

# SQLite
cet_find_library(SQLITE3 NAMES sqlite3_ups PATHS ENV SQLITE_LIB NO_DEFAULT_PATH)

# source code
add_subdirectory(fhiclcpp)
add_subdirectory(python)
add_subdirectory(tools)
add_subdirectory(ups)

# packaging utility
include(UseCPack)
