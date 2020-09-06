# ======================================================================
#
# Testing
#
# ======================================================================

include(CetTest)
cet_enable_asserts()

# Identify libraries to be linked:
link_libraries(fhiclcpp)

cet_test_env("FHICL_FILE_PATH=.")

add_subdirectory(types)

cet_test(dotted_names)
cet_test(hex_test)

cet_test(PSetSample INSTALL_EXAMPLE INSTALL_SOURCE
  DATAFILES Sample.cfg
  )
cet_test(PSetTest)
cet_test(ParameterSet_t USE_BOOST_UNIT
  DATAFILES Sample.cfg
)
cet_test(printing_helpers_t)

cet_test(get_sequence_elements_t USE_BOOST_UNIT
  DATAFILES Sample.cfg
)

cet_test(get_via_t USE_BOOST_UNIT)

cet_test(equalTest USE_BOOST_UNIT)
cet_test(failer DATAFILES test_config_fail.fcl)
cet_test(fhicl-config_t NO_AUTO)
cet_test(fhicl-expand_test.sh PREBUILT)
cet_test(fhicl-expand_test_path.sh PREBUILT)
cet_test(issue_0923_t.sh PREBUILT
  DATAFILES
  issue_0923a.fcl
  issue_0923b.fcl
  issue_0923_ref.txt
  DEPENDENCIES fhicl-config_t
)
cet_test(key_assembler_t USE_BOOST_UNIT)
cet_test(parse_document_test USE_BOOST_UNIT)
cet_test(parse_value_string_test USE_BOOST_UNIT)
cet_test(to_indented_string_test USE_BOOST_UNIT)
cet_test(to_indented_string_annotated_test
  DATAFILES
  issue-7788/to_indented_string_annotated_test.fcl
  issue-7788/to_indented_string_annotated_prolog.fcl
  REF "${CMAKE_CURRENT_SOURCE_DIR}/issue-7788/to_indented_string_annotated_test-ref.txt"
  )
cet_test(to_indented_string_prefix_annotated_test
  DATAFILES
  issue-7788/to_indented_string_annotated_test.fcl
  issue-7788/to_indented_string_annotated_prolog.fcl
  REF "${CMAKE_CURRENT_SOURCE_DIR}/to_indented_string_prefix_annotated_test-ref.txt"
  )
cet_test(to_string_test
  DATAFILES Sample.cfg
)
cet_test(to_tupleTypes_test DATAFILES to_tupleTypes_test.fcl )
cet_test(values_test USE_BOOST_UNIT)

cet_test(test_suite USE_BOOST_UNIT NO_AUTO)
FILE(GLOB testPass RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "testFiles/pass/*_pass.fcl")
#message("${testPass}")
FILE(GLOB testFail RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "testFiles/fail/*_fail.fcl")
#message("${testFail}")
FOREACH(test_file ${testPass})
  GET_FILENAME_COMPONENT(test_file_stem ${test_file} NAME_WE)
  cet_test(${test_file_stem} HANDBUILT
           TEST_EXEC test_suite
           TEST_ARGS -- ${test_file}
           #DATAFILES ${test_file}
           TEST_PROPERTIES ENVIRONMENT FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}
  )
ENDFOREACH()
FOREACH(test_file ${testFail})
  GET_FILENAME_COMPONENT(test_file_stem ${test_file} NAME_WE)
  cet_test(${test_file_stem} HANDBUILT
           TEST_EXEC test_suite
           TEST_ARGS -- ${test_file}
           #DATAFILES ${test_file}
           TEST_PROPERTIES WILL_FAIL true
                           ENVIRONMENT FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}
  )
ENDFOREACH()

cet_test(save-restore_t NO_AUTO)

cet_test(save-restore-ab HANDBUILT
  TEST_EXEC save-restore_t
  TEST_ARGS ab.fcl
  DATAFILES issue-2737-fcl/ab.fcl
)

cet_test(save-restore-ba HANDBUILT
  TEST_EXEC save-restore_t
  TEST_ARGS ba.fcl
  DATAFILES issue-2737-fcl/ba.fcl
)

cet_test(intermediate_table_t USE_BOOST_UNIT)

cet_test(table_t_iterator_t USE_BOOST_UNIT)
get_target_property(DEFS table_t_iterator_t COMPILE_DEFINITIONS)
list(APPEND DEFS SNIPPET_MODE=false)
set_target_properties(table_t_iterator_t PROPERTIES
  COMPILE_DEFINITIONS "${DEFS}")

cet_test(table_t_snippet_iterator_t
  SOURCES table_t_iterator_t.cc
  USE_BOOST_UNIT)
get_target_property(DEFS table_t_snippet_iterator_t COMPILE_DEFINITIONS)
list(APPEND DEFS SNIPPET_MODE=true)
set_target_properties(table_t_snippet_iterator_t PROPERTIES
  COMPILE_DEFINITIONS "${DEFS}")

cet_test(seq_of_seq_t)

cet_test(traits_t)

find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

cet_test(ParameterSetRegistry_t USE_BOOST_UNIT LIBRARIES Threads::Threads)

cet_test(DatabaseSupport_t USE_BOOST_UNIT
         DATAFILES
            testFiles/db_0.fcl
            testFiles/db_1.fcl
            testFiles/db_2.fcl
         )

cet_test(WriteSQLiteDB_t USE_BOOST_UNIT
         DATAFILES testFiles/db_2.fcl
         )

cet_test(fhicl-write-db-test HANDBUILT
  TEST_EXEC sqlite3
  TEST_ARGS ../WriteSQLiteDB_t.d/test.db "select count(ID) from ParameterSets"
  TEST_PROPERTIES
  DEPENDS WriteSQLiteDB_t
  PASS_REGULAR_EXPRESSION "^3\n$"
  )

cet_test(parse_shimmeddocument_test USE_BOOST_UNIT)
