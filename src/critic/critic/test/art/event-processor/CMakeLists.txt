simple_plugin(ThrowingAnalyzer "module" NO_INSTALL)
simple_plugin(ThrowingProducer "module" NO_INSTALL)
simple_plugin(DoNothingInput   "source" NO_INSTALL art_Framework_Services_Registry)

cet_test(ThrowsWithModuleInfo_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c throws_with_module_info_t.fcl
  DATAFILES fcl/throws_with_module_info_t.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "while processing module ThrowingAnalyzer/p1 run: 1 subRun: 0 event: 1")

cet_test(EP_throwing_01_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_01.fcl
  DATAFILES
  fcl/test_01.fcl)

foreach(num RANGE 2 4)
  cet_test(EP_throwing_0${num}_t HANDBUILT
    TEST_EXEC art
    TEST_ARGS -c test_0${num}.fcl
    DATAFILES
    fcl/test_0${num}.fcl
    TEST_PROPERTIES
    PASS_REGULAR_EXPRESSION "Throw from c'tor.")
endforeach()

cet_script(graceful_shutdown_t.sh NO_INSTALL)

cet_test(graceful_shutdown_from_exception_t HANDBUILT
  TEST_EXEC graceful_shutdown_t.sh
  TEST_ARGS -c test_graceful_shutdown.fcl
  REF "${CMAKE_CURRENT_SOURCE_DIR}/graceful_shutdown_from_exception_t-ref.txt"
  OUTPUT_FILTERS "filter-timeTracker-output" "filter-updated-summary"
  DATAFILES
  fcl/test_graceful_shutdown.fcl)

cet_test(do_not_construct_unused_module_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c do_not_construct_unused_module_t.fcl
  DATAFILES fcl/do_not_construct_unused_module_t.fcl)
