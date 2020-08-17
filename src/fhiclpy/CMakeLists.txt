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

cmake_minimum_required(VERSION 3.12...3.14 FATAL_ERROR)
project(fhiclpy LANGUAGES CXX C)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools 7.13.01 REQUIRED)

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
find_ups_product(cetlib_except v1_00_00)
find_ups_product(cetlib v2_01_00)
find_ups_product(fhiclcpp v4_11_01)

add_subdirectory(python)
add_subdirectory(ups)

# packaging utility
include(UseCPack)
