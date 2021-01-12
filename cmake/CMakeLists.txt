# ======================================================================
#  ARIEL cmake modules build file
# ======================================================================

cmake_minimum_required(VERSION 3.12...3.14 FATAL_ERROR)
project(ARIEL_BUILDTOOLS LANGUAGES CXX C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Modules")

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()
set(ARIEL_VERSION ${cet_dot_version})

# Get ROOTSYS (for root modulefile)
find_ups_root()

# Get Python version (for PYTHONPATH)
find_ups_product(python)

# Find Boost to get its include path, which is needed for ROOT_INCLUDE_PATH
find_ups_boost(v1_66_0)

# Currently, this is all we do here: create the environment setup script
include(ArielCreateSetup)
include(ArielCreateModulesfiles)
