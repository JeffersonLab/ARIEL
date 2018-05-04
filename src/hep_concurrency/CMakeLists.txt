# ======================================================================
#  hep_concurrency main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/hep_concurrency/ups/setup_for_development
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


# ----------------------------------------------------------------------
# Describe build tool requirements:

cmake_minimum_required(VERSION 2.8)  # use cmake 2.8 or later

# ----------------------------------------------------------------------
# Describe product:

project(hep_concurrency)

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

# ----------------------------------------------------------------------
# Describe build prerequisites:

find_ups_product(cetbuildtools v6_00_00)
find_ups_product(cppunit)
find_ups_product(tbb)

# define cppunit
cet_find_library(CPPUNIT NAMES cppunit   PATHS ENV CPPUNIT_LIB   NO_DEFAULT_PATH)

# ----------------------------------------------------------------------
# Describe source tree layout:

add_subdirectory(${PROJECT_NAME})  # source code
add_subdirectory(ups)              # ups files

# ----------------------------------------------------------------------
# Packaging utility

include(UseCPack)

#
# ======================================================================
