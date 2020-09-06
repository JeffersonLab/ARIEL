########################################################################
# Verify fixes and/or workaround(s) for
# https://sft.its.cern.ch/jira/projects/ROOT/issues/ROOT-10271.
#
# This subdirectory tree is designed to be usable both in and out of the
# context of canvas_root_io. Under normal circumstances the tests will
# be built and run as part of the build-and-test of the canvas_root_io
# package. In the event that standalone behavior is required, cetmodules
# >= 1.03.00 is required in place of cetmodules. One can use the script:
#
#   standalone/make_standalone_tarball <archive-file-name>
#
# to produce a standalone source distribution (including a suitable
# version of cetmodules) and then utilize the run_standalone script
# and/or setup.sh setup script.
#
# Alternatively, with cetmodules available and set up (and suitable
# versions of ROOT and CMake), one can execute CMake in a suitable build
# directory, pointing CMake to the location of this CMakeLists.txt
# file. An example invocation can be found in standalone/run_standalone.
#
# See subdirectory notes for the significance of the
# ROOT_FIXED_GENERAL_IO_RULES and ROOT_FIXED_DOUBLE32T CMake cache
# variables.
#
####################################
# Subdirectory notes:
####################################
#
# v10/  - Tests of reading and writing the "old" version of the Vertex
#         test collection.
#
# v11/  - Tests of reading and writing the "new" version of the Vertex
#         collection, and of reading the old version from file into the
#         new with a suitable I/O read rule. If the
#         ROOT_FIXED_GENERAL_IO_RULES:BOOL cache variable is at its
#         default value of FALSE, these tests will be marked
#         WILL_FAIL. Set this variable to TRUE to verify that the
#         general issue has in fact been fixed.
#
# v11a/ - Test reading the "old" version of Vertex from file into the
#         new with no I/O rules at all. This will test the specific
#         internal I/O rules that should mediate between double and
#         Double32_t. If the ROOT_FIXED_DOUBLE32T:BOOL cache variable is
#         at its default value of FALSE, these tests will be marked
#         WILL_FAIL.
#
# v11b/ - Test whether in-code read rules added to the TClass will
#         produce the desired behavior.
#
########################################################################

set(ROOT_FIXED_GENERAL_IO_RULES FALSE CACHE BOOL
  "Whether we should expect general nested I/O tests to succeed (v11)")
set(ROOT_FIXED_DOUBLE32T TRUE CACHE BOOL
  "Whether we should expect specific no-read-rule double -> Double32_t \
tests to succeed (v11a)")

set(WANT_XML_READRULES TRUE) # Customize as necessary in subdirectories.
set(WANT_XML_READRULES_CDATA TRUE) # Customize as necessary in subdirectories.

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Using in (almost) standalone mode
  cmake_minimum_required(VERSION 3.11..3.14 FATAL_ERROR)
  project(io_rules_regression_tests VERSION 1.00.00 LANGUAGES CXX)

  # Find and use cetmodules.
  find_package(cetmodules
    1.03.00 REQUIRED # For build_dictionary supporting CLASSES_* opts.
    )
  include(CetCMakeEnv)
  cet_cmake_env()
  cet_set_compiler_flags(DIAGS VIGILANT
    WERROR NO_UNDEFINED
    EXTRA_CXX_FLAGS -Wnon-virtual-dtor -Wdelete-non-virtual-dtor)
  cet_report_compiler_flags()

  # Find and use ROOT.
  find_package(ROOT REQUIRED COMPONENTS)
  include_directories(${ROOT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}/ROOT)
  # Learn how to make dictionaries, executables, tests.
  include (BuildDictionary)
  include (CetMake)
  include (CetTest)
endif()

find_package(ROOT REQUIRED COMPONENTS Core GenVector Hist MathCore RIO Smatrix Tree)

set(VERTEX_SOURCE_IN_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Location of configurable Vertex source.")
set(VERTEX_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Location of configured Vertex source.")

function(configure_vertex VERTEX_VERSION)
  configure_file(${VERTEX_SOURCE_IN_DIR}/Vertex.h.in ${CMAKE_CURRENT_BINARY_DIR}/Vertex.h @ONLY)
  if (VERTEX_VERSION GREATER 10)
    if (WANT_XML_READRULES)
      if (WANT_XML_READRULES_CDATA)
        set(POS_CDATA
          "  <![CDATA[\n\
       pos = arttest::Vertex::Point_t(onfile.pos.X(), onfile.pos.Y(), onfile.pos.Z());\n\
   ]]>\n\
")
        set(COV_DATA
          "  <![CDATA[\n\
       for (size_t i=0; i<3; ++i)
         for (size_t j=0; j<3; ++j)
           cov(i,j) = onfile.cov(i,j);
   ]]>\n\
")
      endif()
      set(VERTEX_IO_RULES
        "<ioread\n\
   version=\"[10]\"\n\
   sourceClass=\"arttest::Vertex\"\n\
   source=\"ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<double>,ROOT::Math::GlobalCoordinateSystemTag> pos;\"\n\
   targetClass=\"arttest::Vertex\"\n\
   target=\"pos\"\n\
   include=\"Vertex.h\"\n\
   >\n\
${POS_CDATA}
  </ioread>\n\
  <ioread\n\
   version=\"[10]\"\n\
   sourceClass=\"arttest::Vertex\"\n\
   source=\"ROOT::Math::SMatrix<Coord_t,3,3,ROOT::Math::MatRepSym<Coord_t,3> > cov;\"\n\
   targetClass=\"arttest::Vertex\"\n\
   target=\"cov\"\n\
   include=\"Vertex.h\"\n\
   >\n\
${COV_CDATA}
  </ioread>\n")
    endif()
    unset(VERTEX_POINT_T)
  else()
    set(VERTEX_POINT_T "<class name=\"arttest::Vertex::Point_t\"/>")
    unset(VERTEX_IO_RULES)
  endif()
  configure_file(${VERTEX_SOURCE_IN_DIR}/classes_def.xml.in ${CMAKE_CURRENT_BINARY_DIR}/classes_def.xml @ONLY)
endfunction()


set(WANTED_ROOT_LIBRARIES
  ${ROOT_Tree_LIBRARY}
  ${ROOT_Hist_LIBRARY}
  ${ROOT_RIO_LIBRARY}
  ${ROOT_Smatrix_LIBRARY}
  ${ROOT_GenVector_LIBRARY}
  ${ROOT_MathCore_LIBRARY}
  ${ROOT_Core_LIBRARY}
)

set(VERTEX_LIBRARIES
  art_root_io_test_io_rules_regression
  ${WANTED_ROOT_LIBRARIES})

function(vertex_tests VERTEX_VERSION)
  if (ARGN)
    string(REPLACE ";" "_" SUFFIX "${ARGN}")
  endif()
  configure_vertex(${VERTEX_VERSION})

  include_directories(${VERTEX_SOURCE_IN_DIR} ${VERTEX_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

  set(v${VERTEX_VERSION}_ROOT_INCLUDE_PATH
    ${CMAKE_CURRENT_FUNCTION_LIST_DIR}:${CMAKE_CURRENT_BINARY_DIR}:$ENV{ROOT_INCLUDE_PATH})

  message(STATUS "v${VERTEX_VERSION}_ROOT_INCLUDE_PATH=${v${VERTEX_VERSION}_ROOT_INCLUDE_PATH}")

  set(v${VERTEX_VERSION}_ROOT_INCLUDE_PATH
    ${v${VERTEX_VERSION}_ROOT_INCLUDE_PATH}
    PARENT_SCOPE)

  message(STATUS "v${VERTEX_VERSION}_ROOT_INCLUDE_PATH=${v${VERTEX_VERSION}_ROOT_INCLUDE_PATH}")

  build_dictionary(DICT_NAME_VAR dictname NO_INSTALL
    CLASSES_DEF_XML ${CMAKE_CURRENT_BINARY_DIR}/classes_def.xml
    CLASSES_H ${CMAKE_CURRENT_SOURCE_DIR}/../classes.h
    CCV_ENVIRONMENT
    "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_CURRENT_BINARY_DIR}"
    "ROOT_INCLUDE_PATH=${CMAKE_CURRENT_FUNCTION_LIST_DIR}:${CMAKE_CURRENT_BINARY_DIR}:$ENV{ROOT_INCLUDE_PATH}"
    )

  set_property(TARGET
    ${dictname}_dict
    PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

  if (NOT TARGET ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_writer)
    foreach(WRAPPED "" "wrapped_")
      foreach(stem reader writer)
        configure_file(${VERTEX_SOURCE_IN_DIR}/vertex_${stem}.cc.in
          ${CMAKE_CURRENT_BINARY_DIR}/${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_${stem}.cc @ONLY)
        cet_make_exec(${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_${stem}
          SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_${stem}.cc
          LIBRARIES ${VERTEX_LIBRARIES})

        set_property(TARGET
          ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_${stem}
          PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
      endforeach()

      cet_test(${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_w HANDBUILT
        TEST_EXEC ${CMAKE_CURRENT_BINARY_DIR}/${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_writer
        TEST_ARGS ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}.root
        REMOVE_ON_FAILURE ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}.root)

      cet_test(${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_r HANDBUILT
        TEST_EXEC ${CMAKE_CURRENT_BINARY_DIR}/${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_reader
        TEST_ARGS ../${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_w.d/${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}.root
        REQUIRED_FILES ../${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_w.d/${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}.root
        REF ${CMAKE_CURRENT_SOURCE_DIR}/../vertex-ref.txt)

      set_property(TEST
        ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_w
        ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_r
        PROPERTY ENVIRONMENT
        LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_CURRENT_BINARY_DIR})

      set_property(TEST
        ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_w
        ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_r
        APPEND
        PROPERTY ENVIRONMENT
        ROOT_INCLUDE_PATH=${v${VERTEX_VERSION}_ROOT_INCLUDE_PATH}
        )

      set_property(TEST
        ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_r
        PROPERTY DEPENDS ${WRAPPED}vertex_v${VERTEX_VERSION}${SUFFIX}_w)
    endforeach()
  endif()
endfunction()

cet_make_library(LIBRARY_NAME art_root_io_test_io_rules_regression
  SOURCE setupRoot.cc
  LIBRARIES ${WANTED_ROOT_LIBRARIES}
  NO_INSTALL)

# Schema evolution
add_subdirectory(v10)
add_subdirectory(v11)
add_subdirectory(v11a)
add_subdirectory(v11b)
