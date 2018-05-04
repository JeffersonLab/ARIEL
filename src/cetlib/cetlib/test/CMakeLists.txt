# ======================================================================
#
# Testing
#
# ======================================================================

include(CetTest)
cet_enable_asserts()
cet_test_env(CURRENT_DIR=${CMAKE_CURRENT_SOURCE_DIR})

# Identify libraries to be linked:
link_libraries(cetlib)

add_subdirectory(sqlite)
cet_test(assert_only_one_thread_test
  LIBRARIES pthread
  TEST_PROPERTIES PASS_REGULAR_EXPRESSION "Failed assert--more than one thread accessing location")
cet_test(bit_test)
cet_test(base_converter_test)
cet_test(canonical_string_test USE_BOOST_UNIT)
cet_test(column_width_test USE_BOOST_UNIT)
cet_test(container_algs_test USE_BOOST_UNIT)
cet_test(cpu_timer_test USE_BOOST_UNIT
  TEST_PROPERTIES RUN_SERIAL true
  OPTIONAL_GROUPS LOAD_SENSITIVE
  )
cet_test(exempt_ptr_test)
cet_test(filepath_maker_test USE_BOOST_UNIT
  TEST_PROPERTIES
  ENVIRONMENT FILEPATH_MAKER_TEST_FILES=${CMAKE_CURRENT_SOURCE_DIR}/filepath_maker-files
  DATAFILES
  filepath_maker_test.txt)
cet_test(filesystem_test)
cet_test(getenv_test)
cet_test(inc-expand_test.sh PREBUILT DEPENDENCIES inc-expand)
cet_test(include_test)
cet_test(includer_test USE_BOOST_UNIT LIBRARIES)
cet_test(is_absolute_filepath_t USE_BOOST_UNIT)
cet_test(lpad_test USE_BOOST_UNIT)
cet_test(map_vector_test USE_BOOST_UNIT)
cet_test(maybe_ref_test USE_BOOST_UNIT)
cet_test(MD5Digest_test)
cet_test(name_of_test USE_BOOST_UNIT)
cet_test(no_delete_t USE_BOOST_UNIT)
cet_test(nybbler_test)
cet_test(os_libpath_t)
cet_test(ostream_handle_test USE_BOOST_UNIT)
cet_test(pow_test USE_BOOST_UNIT)
cet_test(pow_constexpr_test)
cet_test(registry_test)
cet_test(registry_via_id_test)
cet_test(registry_via_id_test_2 NO_AUTO) # for now -- see test's source
cet_test(rpad_test USE_BOOST_UNIT)
cet_test(search_path_test TEST_PROPERTIES ENVIRONMENT xyzzy="")
cet_test(search_path_test_2 NO_AUTO)
cet_test(search_path_test_2.sh PREBUILT DEPENDENCIES search_path_test_2)
cet_test(search_path_test_3 USE_BOOST_UNIT)
cet_test(simultaneous_function_spawner_t USE_BOOST_UNIT LIBRARIES pthread )
cet_test(sha1_test)
cet_test(sha1_test_2 SOURCES sha1_test_2.cc sha1.cpp)
cet_test(sha1_test_performance NO_AUTO SOURCES sha1_test_performance.cc sha1.cpp)
cet_test(shlib_utils_t USE_BOOST_UNIT)
cet_test(simple_stats_t USE_BOOST_UNIT)
cet_test(split_by_regex_test USE_BOOST_UNIT)
cet_test(split_path_test)
cet_test(split_test USE_BOOST_UNIT)
cet_test(trim_test USE_BOOST_UNIT)
cet_test(value_ptr_test USE_BOOST_UNIT)
cet_test(value_ptr_test_2)
cet_test(value_ptr_test_3)
cet_test(value_ptr_test_4)
cet_test(value_ptr_test_5)

cet_make_library(LIBRARY_NAME cetlib_test_fakePlugin SOURCE moduleType.cc NO_INSTALL)

cet_make_library(LIBRARY_NAME cetlib_test_TestPluginBase SOURCE TestPluginBase.cc NO_INSTALL)

include(BasicPlugin)
basic_plugin(TestPlugin "plugin" NO_INSTALL cetlib_test_TestPluginBase)

# Use default Plugin lookup
cet_test(PluginFactory_t USE_BOOST_UNIT
  LIBRARIES cetlib cetlib_test_TestPluginBase
)

# Use custom Plugin lookup
cet_test(PluginFactoryCustomSearchPath_t USE_BOOST_UNIT
  SOURCES PluginFactory_t.cc
  LIBRARIES cetlib cetlib_test_TestPluginBase
  TEST_PROPERTIES ENVIRONMENT PLUGIN_FACTORY_SEARCH_PATH=$<TARGET_FILE_DIR:cetlib_test_TestPlugin_plugin>
  )
target_compile_definitions(PluginFactoryCustomSearchPath_t PRIVATE PLUGIN_FACTORY_SEARCH_PATH=1)

function(test_library LIBSPEC)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${LIBSPEC}.cc
    COMMAND ${CMAKE_COMMAND}
    -DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DBIN_DIR="${CMAKE_CURRENT_BINARY_DIR}"
    -DLIBSPEC="${LIBSPEC}"
    -P ${CMAKE_CURRENT_SOURCE_DIR}/configureLibraryManagerTestFunc.cmake
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/configureLibraryManagerTestFunc.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/LibraryManagerTestFunc.cc.in
    )
  add_library(${LIBSPEC}_cetlibtest SHARED ${CMAKE_CURRENT_BINARY_DIR}/${LIBSPEC}.cc)
endfunction()

test_library(1_1_1)
test_library(1_1_2)
test_library(1_1_3)
test_library(1_2_3)
test_library(2_1_5)

# Use default library search path
cet_test(LibraryManager_t USE_BOOST_UNIT
  LIBRARIES
  ${Boost_FILESYSTEM_LIBRARY}
  ${CMAKE_DL_LIBS}
  )

# Use custom library search path
cet_test(LibraryManagerCustomSearchPath_t USE_BOOST_UNIT
  SOURCES LibraryManager_t.cc
  LIBRARIES
  ${Boost_FILESYSTEM_LIBRARY}
  ${CMAKE_DL_LIBS}
  TEST_PROPERTIES ENVIRONMENT LIBRARY_MANAGER_SEARCH_PATH=$<TARGET_FILE_DIR:1_1_1_cetlibtest>
  )
target_compile_definitions(LibraryManagerCustomSearchPath_t PRIVATE LIBRARY_MANAGER_SEARCH_PATH=1)

cet_test(replace_all_test USE_BOOST_UNIT
  LIBRARIES
  cetlib
  )

cet_test(regex_t USE_BOOST_UNIT
  LIBRARIES
  cetlib
  DATAFILES regex.txt)

cet_test(regex_standalone_t
  SOURCES regex_t.cc
  LIBRARIES
  cetlib
  DATAFILES regex.txt)

set_target_properties(regex_standalone_t PROPERTIES COMPILE_DEFINITIONS STANDALONE_TEST)

########################################################################
# Demonstration of Catch unit tests.

# Simple test (standard TEST_CASE usage).
cet_test(canonical_number_test USE_CATCH_MAIN)

# Simple test (BDD-style usage) with test details turned on.
cet_test(crc32_test USE_CATCH_MAIN SOURCES crc32_test.cc CRC32Calculator.cc TEST_ARGS -s)

# Use ParseAndAddCatchTests to generate a test for each test case.
cet_test(hypot_test USE_CATCH_MAIN NO_AUTO)
list(APPEND CMAKE_MODULE_PATH $ENV{CATCH_DIR}/share/cmake/catch)
include(ParseAndAddCatchTests)
set(AdditionalCatchParameters -s)
ParseAndAddCatchTests(hypot_test)
