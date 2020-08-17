cmake_minimum_required(VERSION 3.14..3.18 FATAL_ERROR)
project(r10870 LANGUAGES CXX C)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  message(STATUS "PROJECT_NAME = ${PROJECT_NAME}")
  # Standalone.
  find_package(cetbuildtools 7.17.00 REQUIRED)

  include(CetCMakeEnv)
  cet_cmake_env()

  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)

  cet_set_compiler_flags(DIAGS VIGILANT
    WERROR
    NO_UNDEFINED
    EXTRA_FLAGS -pedantic
    EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -DBOOST_TEST_NO_OLD_TOOLS)

  cet_report_compiler_flags()

  find_package(ROOT 6.20.00 REQUIRED HINTS $ENV{ROOTSYS})
  ####################################
  # Shim to dictionary-building code that relies on old find_ups_root().
  set(HAVE_ROOT6 TRUE)
  set(ROOT_CORE ROOT::Core)
  set(ROOT_REFLEX)
  set(ROOT_GENREFLEX ${ROOT_genreflex_CMD})
  message(STATUS "ROOT_GENREFLEX = ${ROOT_GENREFLEX}")
  include(ArtDictionary)
  include(ArtMake)
  cet_enable_asserts()
  include(CetTest)
else()
  project(r10870 LANGUAGES CXX C)
endif()

art_make_library(LIBRARY_NAME ${PROJECT_NAME}
  SOURCE DataObj.cc Exception.cc exception.cc
  NO_INSTALL)
art_dictionary(${PROJECT_NAME}
  NO_DEFAULT_LIBRARIES NO_INSTALL
  DICT_NAME_VAR dictname
  DICTIONARY_LIBRARIES ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_dict
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_custom_target(${PROJECT_NAME}-reproducer_tar
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${PROJECT_NAME}-reproducer"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}"
  "${PROJECT_NAME}-reproducer"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "$ENV{CETBUILDTOOLS_DIR}"
  "${PROJECT_NAME}-reproducer/cetbuildtools/$ENV{CETBUILDTOOLS_VERSION}"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "$ENV{CETPKGSUPPORT_DIR}"
  "${PROJECT_NAME}-reproducer/cetpkgsupport/$ENV{CETPKGSUPPORT_VERSION}"
  COMMAND ${CMAKE_COMMAND} -E tar jcf "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-reproducer.tar.bz2" "${PROJECT_NAME}-reproducer"
  COMMENT "Generating self-contained archive ${PROJECT_NAME}-reproducer.tar.bz2"
  )
