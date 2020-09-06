# ======================================================================
#  catch main build file
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

project(catch)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()

# ----------------------------------------------------------------------
# Describe build prerequisites:


# ----------------------------------------------------------------------
# Describe source tree layout:

add_subdirectory( ${PROJECT_NAME} )  # source code
add_subdirectory( ups )              # ups files

# ----------------------------------------------------------------------
# Patch and install contributed ParseAndAddCatchTests module

set(module_dir "${PROJECT_BINARY_DIR}/Modules")
set(parse_and_add_module "ParseAndAddCatchTests.cmake")
file(MAKE_DIRECTORY ${module_dir})
execute_process( COMMAND patch -p1
  -d "${PROJECT_SOURCE_DIR}/catch"
  -i "${PROJECT_SOURCE_DIR}/patch/AddTargets.patch"
  -o "${module_dir}/${parse_and_add_module}"
  RESULT_VARIABLE ec
  OUTPUT_QUIET
  )
if(ec)
  message(FATAL_ERROR "Error patching ${parse_and_add_module}")
endif(ec)

# Catch2 does not install anything when used as a subproject.
# As a temporary workaround, until the entire suite is built as
# one unit, we install things from here.
install(
  DIRECTORY
  "catch/single_include/"
  DESTINATION
  "${CMAKE_INSTALL_INCLUDEDIR}"
  )

# Install CMake scripts
install(
  FILES
  "${module_dir}/${parse_and_add_module}"
  "catch/contrib/Catch.cmake"
  "catch/contrib/CatchAddTests.cmake"
  DESTINATION
  "${${product}_data_dir}/Modules"
  )

# ----------------------------------------------------------------------
# Packaging utility

include(UseCPack)

#
# ======================================================================
