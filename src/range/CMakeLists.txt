# ======================================================================
#  range main build file
#
#  cd .../path/to/build/directory
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        ../path/to/source/directory
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


# ----------------------------------------------------------------------
# Describe build tool requirements:

cmake_minimum_required(VERSION 3.0)

# ----------------------------------------------------------------------
# Describe product:

project(range)

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

set(RANGES_CXX_STD "" CACHE STRING "" FORCE)
string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )
string(REGEX MATCH "(^| )-std=[^ ]*\\+\\+([^ ]*)" std_flag "${CMAKE_CXX_FLAGS_${BTYPE_UC}}")
set(RANGES_CXX_STD "${CMAKE_MATCH_2}")

option(RANGE_V3_NO_TESTING "" TRUE)
option(RANGE_V3_NO_EXAMPLE "" TRUE)
option(RANGE_V3_NO_PERF "" TRUE)

# ----------------------------------------------------------------------
# Describe build prerequisites:

# ----------------------------------------------------------------------
# Replace top-level CMakeLists.txt of range submodule with our patched
# version. (Don't build tests. Don't install configuration scripts, we
# have our own)

file(COPY patches/range_CMakeLists.txt
     DESTINATION ${PROJECT_SOURCE_DIR}/range)
file(RENAME ${PROJECT_SOURCE_DIR}/range/range_CMakeLists.txt
            ${PROJECT_SOURCE_DIR}/range/CMakeLists.txt)

# ----------------------------------------------------------------------
# Describe source tree layout:

add_subdirectory( ${PROJECT_NAME} )  # source code
add_subdirectory( ups )              # ups files

# ----------------------------------------------------------------------
# Packaging utility

include(UseCPack)

#
# ======================================================================
