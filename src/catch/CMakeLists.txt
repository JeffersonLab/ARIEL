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
set(module "ParseAndAddCatchTests.cmake")
file(MAKE_DIRECTORY ${module_dir})
execute_process( COMMAND patch -p1
  -d ${PROJECT_SOURCE_DIR}/catch
  -i ${PROJECT_SOURCE_DIR}/patch/AddTargets.patch
  -o ${module_dir}/${module}
  RESULT_VARIABLE ec
  OUTPUT_QUIET
  )
if(ec)
  message(FATAL_ERROR "Error patching ${module}")
endif(ec)

install( FILES ${module_dir}/${module} DESTINATION ${${product}_data_dir}/Modules )

# ----------------------------------------------------------------------
# Packaging utility

include(UseCPack)

#
# ======================================================================
