find_package(Threads REQUIRED)

simple_plugin(AssnsAnalyzer               "module" USE_BOOST_UNIT NO_INSTALL)
simple_plugin(AssnsProducer               "module" NO_INSTALL)
simple_plugin(BareStringAnalyzer          "module" NO_INSTALL)
simple_plugin(BareStringProducer          "module" NO_INSTALL)
simple_plugin(CompressedIntProducer       "module" NO_INSTALL)
simple_plugin(CompressedIntTestAnalyzer   "module" NO_INSTALL)
simple_plugin(DefaultEngineTester         "module" NO_INSTALL ${CLHEP})
simple_plugin(DerivedPtrVectorAnalyzer    "module" NO_INSTALL)
simple_plugin(DerivedPtrVectorProducer    "module" NO_INSTALL)
simple_plugin(DifferentHistoriesAnalyzer  "module" USE_BOOST_UNIT NO_INSTALL)
simple_plugin(DoubleProducer              "module" NO_INSTALL)
simple_plugin(DoubleTestAnalyzer          "module" NO_INSTALL)
simple_plugin(DropTestAnalyzer            "module" USE_BOOST_UNIT NO_INSTALL)
simple_plugin(DropTestParentageFaker      "module" NO_INSTALL)
simple_plugin(FailingProducer             "module" NO_INSTALL)
simple_plugin(IntProducer                 "module" NO_INSTALL)
simple_plugin(IntReaderThenProducer       "module" NO_INSTALL)
simple_plugin(IntTestAnalyzer             "module" NO_INSTALL)
simple_plugin(IntVectorAnalyzer           "module" NO_INSTALL)
simple_plugin(IntVectorProducer           "module" NO_INSTALL)
simple_plugin(MockClusterListAnalyzer     "module" NO_INSTALL)
simple_plugin(MockClusterListProducer     "module" NO_INSTALL)
simple_plugin(ProductIDGetter             "module" USE_BOOST_UNIT NO_INSTALL)
simple_plugin(ProductIDGetterAnalyzer     "module" USE_BOOST_UNIT NO_INSTALL)
simple_plugin(PtrmvAnalyzer               "module" USE_BOOST_UNIT NO_INSTALL)
simple_plugin(PtrmvProducer               "module" NO_INSTALL)
simple_plugin(PtrVectorSimpleAnalyzer     "module" NO_INSTALL)
simple_plugin(RandomNumberSaveTest        "module" USE_BOOST_UNIT NO_INSTALL ${CLHEP})
simple_plugin(RunSubRunProductProducerNoPut "module" NO_INSTALL)
simple_plugin(SAMMetadataTest             "module"
  NO_INSTALL
  art_Framework_Services_System_FileCatalogMetadata_service
  art_Framework_Services_Registry)
simple_plugin(SimpleDerivedAnalyzer       "module" NO_INSTALL)
simple_plugin(SimpleDerivedProducer       "module" NO_INSTALL)
simple_plugin(TestFilterSpecificEvents    "module" NO_INSTALL)
simple_plugin(TestProvenanceDumper        "module" USE_BOOST_UNIT NO_INSTALL)
simple_plugin(ToyProductProducer          "module" NO_INSTALL)
simple_plugin(ToyRawProductAnalyzer       "module" NO_INSTALL)
simple_plugin(ValidHandleTester           "module" NO_INSTALL)
simple_plugin(VerifyTransitionsLegacy     "module" USE_BOOST_UNIT NO_INSTALL)
simple_plugin(VerifyTransitionsShared     "module" USE_BOOST_UNIT NO_INSTALL)
simple_plugin(VerifyTransitionsReplicated "module" USE_BOOST_UNIT NO_INSTALL)

simple_plugin(AddIntsProducer             "module" NO_INSTALL)
simple_plugin(AssnsMultipleProduces       "module" NO_INSTALL)
simple_plugin(AssnsWrongPut               "module" NO_INSTALL)
simple_plugin(ConcurrentEngineRetrieval   "module" NO_INSTALL USE_BOOST_UNIT ${CLHEP})
simple_plugin(DeferredIsReadyWithAssnsAnalyzer "module" NO_INSTALL)
simple_plugin(DeferredIsReadyWithAssnsProducer "module" NO_INSTALL)
simple_plugin(EventIDFilterChecker        "module" NO_INSTALL)
simple_plugin(FailingAnalyzer             "module" NO_INSTALL)
simple_plugin(UnputtingProducer           "module" NO_INSTALL)
simple_plugin(FlushingGenerator           "source" NO_INSTALL)
simple_plugin(FlushingGeneratorTest       "module" NO_INSTALL)
simple_plugin(FlushingGeneratorTestFilter "module" NO_INSTALL)
simple_plugin(GeneratorTest               "source" NO_INSTALL)
simple_plugin(ImplicitRSAssignmentAnalyzer "module" NO_INSTALL USE_BOOST_UNIT)
simple_plugin(ImplicitRSAssignmentProducer "module" NO_INSTALL USE_BOOST_UNIT)
simple_plugin(IntsVerifier                "module" NO_INSTALL)
simple_plugin(MessagePrinter              "tool"   NO_INSTALL)
simple_plugin(ProductIDGetterNoPut        "module" NO_INSTALL USE_BOOST_UNIT)
simple_plugin(PtrListAnalyzer             "module" NO_INSTALL)
simple_plugin(ReadNextArbitrator          "tool"   NO_INSTALL)

simple_plugin(SAMMetadataConcurrentTest   "module"
  NO_INSTALL USE_BOOST_UNIT
  art_Framework_Services_Registry
  art_Framework_Services_System_TriggerNamesService_service
  art_Framework_Services_System_FileCatalogMetadata_service
  Threads::Threads)

simple_plugin(ServiceUsing                "service" NO_INSTALL hep_concurrency)
simple_plugin(SigIntAnalyzer              "module" NO_INSTALL)
simple_plugin(SimpleSource                "source" NO_INSTALL)
simple_plugin(TestAnalyzerSelect          "module" NO_INSTALL MF_MessageLogger)
simple_plugin(TestBitsOutput              "module" NO_INSTALL)
simple_plugin(TestFilter                  "module" NO_INSTALL)
simple_plugin(TestOutput                  "module" NO_INSTALL MF_MessageLogger)
simple_plugin(TestResultAnalyzer          "module" NO_INSTALL MF_MessageLogger)
simple_plugin(TestServiceUsingService     "module" NO_INSTALL USE_BOOST_UNIT art_Framework_Services_Registry)
simple_plugin(TestSimpleMemoryCheckProducer "module" NO_INSTALL)
simple_plugin(TestTimeTrackerProducer     "module" NO_INSTALL)
simple_plugin(TestTimeTrackerFilter       "module" NO_INSTALL)
simple_plugin(TestTimeTrackerAnalyzer     "module" NO_INSTALL)
simple_plugin(Throwing                    "service" NO_INSTALL hep_concurrency MF_MessageLogger)
simple_plugin(ToyProductAnalyzer          "module" NO_INSTALL)
simple_plugin(ToyProductFilter            "module" NO_INSTALL)
simple_plugin(ToyProductFilterReplicated  "module" NO_INSTALL)
simple_plugin(ToyProductFilterAsync       "module" NO_INSTALL)
simple_plugin(ToyProductProducerMultiput  "module" NO_INSTALL)
simple_plugin(U_S                         "module" NO_INSTALL ALLOW_UNDERSCORES)
simple_plugin(Wanted                      "service" NO_INSTALL)
simple_plugin(addInts                     "tool"   NO_INSTALL)

########################################################################
# Old tests converted from CMS FWCore/Framework/test
set(test_list
  BitsPass_t
  BitsFail_t
  BitsCount_t
  FilterIgnore_t
  TrigBits0_t
  TrigBits1_t
  TrigBits2_t
  TrigBits3_t
  TrigBits4_t)

foreach(test ${test_list})
  cet_test(${test} HANDBUILT
    TEST_EXEC art
    TEST_ARGS -c ${test}.fcl
    DATAFILES
    fcl/${test}.fcl
    fcl/messageDefaults.fcl)
endforeach()
########################################################################

set(NUM_THREADS 1 2 4 8)

cet_test(EventIDFilter_t HANDBUILT
  TEST_EXEC art
  PARG_J -j ${NUM_THREADS}
  TEST_ARGS --config EventIDFilter_t.fcl -n 10
  DATAFILES fcl/EventIDFilter_t.fcl)

cet_test(GetProductsCrossPaths_t1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c getProductsCrossPaths_t1.fcl
  DATAFILES fcl/getProductsCrossPaths_t1.fcl)

cet_test(GetProductsCrossPaths_t2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c getProductsCrossPaths_t2.fcl
  DATAFILES
  fcl/getProductsCrossPaths_t1.fcl
  fcl/getProductsCrossPaths_t2.fcl
  TEST_PROPERTIES WILL_FAIL TRUE)

cet_test(GetManyByType_t1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c getManyByType_t1.fcl
  DATAFILES fcl/getManyByType_t1.fcl)

cet_test(GetProductDuplicateModuleName_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c getProductDuplicateModuleName_w.fcl -o out.root
  DATAFILES fcl/getProductDuplicateModuleName_w.fcl)

cet_test(GetProductDuplicateModuleName_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c getProductDuplicateModuleName_t.fcl
  DATAFILES fcl/getProductDuplicateModuleName_t.fcl
  REQUIRED_FILES ../GetProductDuplicateModuleName_w.d/out.root
  TEST_PROPERTIES DEPENDS GetProductDuplicateModuleName_w)

cet_test(ptr_list_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config ptr_list_01.fcl
  DATAFILES
  fcl/ptr_list_01.fcl
  fcl/messageDefaults.fcl)

cet_test(test_unputtingProducer_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_unputtingProducer.fcl
  DATAFILES
  fcl/test_unputtingProducer.fcl
  fcl/messageDefaults.fcl
  TEST_PROPERTIES WILL_FAIL TRUE)

cet_test(issue_0923_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c issue_0923b.fcl --debug-config config.out
  DATAFILES
  fcl/issue_0923a.fcl
  fcl/issue_0923b.fcl)

cet_test(issue_0923_r HANDBUILT
  TEST_EXEC diff
  TEST_ARGS -u ${CMAKE_CURRENT_SOURCE_DIR}/issue_0923_ref.txt ../issue_0923_w.d/config.out
  TEST_PROPERTIES DEPENDS issue_0923_w)

cet_test(issue_0940_t.sh PREBUILT
  DATAFILES
  fcl/issue_0940.fcl
  fcl/messageDefaults.fcl)

cet_test(issue_0930_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c issue_0930.fcl
  DATAFILES
  fcl/issue_0930.fcl
  fcl/messageDefaults.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "LogicError BEGIN")

cet_test(ToyProductProducerFilter_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c toyProductProducerFilter_t.fcl
  DATAFILES fcl/toyProductProducerFilter_t.fcl)

cet_test(ToyProductProducerFilterReplicated_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyProductProducerFilterReplicated_t.fcl -j 10
  DATAFILES fcl/ToyProductProducerFilterReplicated_t.fcl)

cet_test(ToyProductProducerFilterAsync_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyProductProducerFilterAsync_t.fcl
  DATAFILES fcl/ToyProductProducerFilterAsync_t.fcl)

cet_test(ToyProductProducerAnalyzer_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c toyProductProducerAnalyzer_t.fcl
  DATAFILES fcl/toyProductProducerAnalyzer_t.fcl)

# The following test throws an exception and does not return status
# code 0.  The test is checking that the correct exception has been
# thrown, as indicated by the regular-expression pattern that matches
# the exception message.
cet_test(SimpleSource_t_01 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c SimpleSource_t_01.fcl
  DATAFILES fcl/SimpleSource_t_01.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "readNext returned true but no RunPrincipal has been set")

foreach(num 01 02 03)
  cet_test(ToyProductProducerMultiput_t_${num} HANDBUILT
    TEST_EXEC art
    TEST_ARGS -c ToyProductProducerMultiput_t_${num}.fcl
    DATAFILES fcl/ToyProductProducerMultiput_t_${num}.fcl
    TEST_PROPERTIES
    PASS_REGULAR_EXPRESSION "Attempt to put multiple products with the"
    PASS_REGULAR_EXPRESSION "following description onto the")
endforeach()

cet_test(ProductIDGetter_t HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "ProductIDGetter_t.fcl"
  DATAFILES fcl/ProductIDGetter_t.fcl)

cet_test(ProductIDGetterNoPut_t_01 HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "ProductIDGetterNoPut_t_01.fcl"
  DATAFILES fcl/ProductIDGetterNoPut_t_01.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "The following products have been declared with 'produces'")

cet_test(ProductIDGetterNoPut_t_02 HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "ProductIDGetterNoPut_t_02.fcl"
  DATAFILES fcl/ProductIDGetterNoPut_t_02.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "The following products have been declared with 'produces'")

cet_test(DeferredIsReadyWithAssns_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config "DeferredIsReadyWithAssns_t.fcl"
  DATAFILES fcl/DeferredIsReadyWithAssns_t.fcl)

# Write a data file containing stored products and stored random number
# states for subsequent tests.
cet_test(RNGConcurrentAccess_t HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "RNGConcurrentAccess_t.fcl"
  DATAFILES fcl/RNGConcurrentAccess_t.fcl)

cet_test(AssnsWrongPut_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c "AssnsWrongPut_t.fcl"
  DATAFILES fcl/AssnsWrongPut_t.fcl
  TEST_PROPERTIES
  # std::size_t may have different representations on different
  # platforms.  Therefore we just require that it be unsigned, and
  # wildcard ('.*') the rest of it.
  PASS_REGULAR_EXPRESSION "No product is registered for.*art::Assns<std::string,unsigned.*,void>")

cet_test(AssnsMultipleProduces_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c "AssnsMultipleProduces_t.fcl"
  DATAFILES fcl/AssnsMultipleProduces_t.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "That friendly name has already been registered for this module.")

cet_test(ServiceUsingService_t HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "ServiceUsingService_t.fcl"
  DATAFILES fcl/ServiceUsingService_t.fcl)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  cet_test(MemoryTracker_t HANDBUILT
    TEST_EXEC art
    PARG_J -j ${NUM_THREADS}
    TEST_ARGS -c memoryTracker.fcl
    DATAFILES
    fcl/memoryTracker.fcl
    fcl/messageDefaults.fcl
    TEST_PROPERTIES
    PASS_REGULAR_EXPRESSION "Details saved in.*passed = 10.*status 0\\.")
endif()

cet_test(TimeTracker_t1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c TimeTracker_t1.fcl
  DATAFILES fcl/TimeTracker_t1.fcl
  REF "${CMAKE_CURRENT_SOURCE_DIR}/TimeTracker_t1-ref.txt"
  OUTPUT_FILTERS "filter-timeTracker-output" "filter-updated-summary")

# Exercise TimeTracker's parallel execution handling
cet_test(TimeTracker_t2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c TimeTracker_t2.fcl -j10
  DATAFILES
  fcl/TimeTracker_t2.fcl
  fcl/TimeTracker_t1.fcl)

cet_test(SAM_metadata_concurrent_insertion HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "SAMMetadata_concurrent_insertion_t.fcl"
  DATAFILES fcl/SAMMetadata_concurrent_insertion_t.fcl)

cet_test(GeneratorSource_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c "GeneratorSource_t.fcl"
  DATAFILES fcl/GeneratorSource_t.fcl)

cet_test(FlushingGenerator_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c "FlushingGenerator_t.fcl"
  DATAFILES fcl/FlushingGenerator_t.fcl)

cet_test(BlockingPrescaler_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config BlockingPrescaler_t.fcl
  REF "${CMAKE_CURRENT_SOURCE_DIR}/BlockingPrescaler_t-ref.txt"
  DATAFILES fcl/BlockingPrescaler_t.fcl
  OUTPUT_FILTERS "filter-updated-summary" "DEFAULT")

# Test signal handling: specifically, check that whenever a signal is
# received, the art framework appropriately backs out of the event
# processing level.  We set errorOnSIGINT to false so that we need
# only one STDOUT reference file (i.e. return code is 0).
foreach(SIGNAL sigint sigquit sigterm sigusr2)
  cet_test(signal_handling_${SIGNAL}_t HANDBUILT
    TEST_EXEC art
    TEST_ARGS -c signal_${SIGNAL}.fcl --errorOnSIGINT=false --process-name="Test${SIGNAL}"
    REF "${CMAKE_CURRENT_SOURCE_DIR}/signal_handling_t-ref.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/signal_handling_${SIGNAL}_rc-ref.txt"
    DATAFILES
    fcl/signal_base.fcl
    fcl/signal_${SIGNAL}.fcl
    OUTPUT_FILTERS "filter-updated-summary")
endforeach()

cet_test(ImplicitRSAssignment_t HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- --config implicitRSAssignment_t.fcl
  DATAFILES fcl/implicitRSAssignment_t.fcl)

cet_test(TestAnalyzerSelect HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c "TestAnalyzerSelect.fcl"
  DATAFILES fcl/TestAnalyzerSelect.fcl)

cet_test(UnusedModule_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c FilterIgnore_t.fcl
  DATAFILES fcl/FilterIgnore_t.fcl)

SET_TESTS_PROPERTIES(UnusedModule_t PROPERTIES
  FAIL_REGULAR_EXPRESSION "not assigned to any path")

cet_test(MissingModuleType_t1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c MissingModuleType_t1.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION
  "ERROR: Configuration of module with label BadModuleConfig encountered the following error:\n"
  DATAFILES fcl/MissingModuleType_t1.fcl)

cet_test(MissingModuleType_t2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c MissingModuleType_t2.fcl
  DATAFILES fcl/MissingModuleType_t2.fcl)

set_source_files_properties(TestEmptyEventTimestampNoRSRTS_plugin.cc
  PROPERTIES COMPILE_DEFINITIONS TEST_USE_LAST_EVENT_TIMESTAMP)

simple_plugin(TestEmptyEventTimestamp "plugin" NO_INSTALL
  art_Framework_Core
  art_Framework_Principal
  fhiclcpp
  Boost::filesystem)

simple_plugin(TestEmptyEventTimestampNoRSRTS "plugin" NO_INSTALL
  art_Framework_Core
  art_Framework_Principal
  fhiclcpp
  Boost::filesystem)

cet_test(TestEmptyEventTimestamp_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c TestEmptyEventTimestamp_t.fcl
  DATAFILES fcl/TestEmptyEventTimestamp_t.fcl)

cet_test(TestEmptyEventTimestampNoRSRTS_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c TestEmptyEventTimestampNoRSRTS_t.fcl
  DATAFILES
  fcl/TestEmptyEventTimestamp_t.fcl
  fcl/TestEmptyEventTimestampNoRSRTS_t.fcl)

simple_plugin(SimpleServiceTest "service" NO_INSTALL)
simple_plugin(SimpleServiceTestAnalyzer "module" NO_INSTALL critic_test_art_SimpleServiceTest_service)

cet_test(SimpleServiceTest_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c SimpleServiceTest_t.fcl
  DATAFILES fcl/SimpleServiceTest_t.fcl)

add_subdirectory(assns-iter)
add_subdirectory(assns-reader)
add_subdirectory(event-processor)
add_subdirectory(event-shape)
add_subdirectory(high-memory)
add_subdirectory(output-file-handling)
add_subdirectory(producing-services)
add_subdirectory(product-aggregation)
add_subdirectory(product-mixing)
add_subdirectory(ptr-maker)
add_subdirectory(ptr-vector)
add_subdirectory(random-numbers)
add_subdirectory(range-algorithms)
add_subdirectory(range-sets)
add_subdirectory(sam-metadata)
add_subdirectory(sampling-input)
add_subdirectory(secondary-input-files)
add_subdirectory(seek-to-event)
add_subdirectory(toy-raw-input)

cet_test(DataFlowDumper_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c data_flow_dumper_t.fcl -n 1
  DATAFILES fcl/data_flow_dumper_t.fcl
  TEST_PROPERTIES DEPENDS test_tiered_input_05d_t)

cet_test(DefaultEngineTester_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c default_engine_t.fcl -n 1
  DATAFILES fcl/default_engine_t.fcl)

cet_test(BareString_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c BareString_w.fcl
  DATAFILES fcl/BareString_w.fcl)

cet_test(BareString_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c BareString_r.fcl
  DATAFILES fcl/BareString_r.fcl
  REQUIRED_FILES "../BareString_w.d/out.root"
  TEST_PROPERTIES DEPENDS BareString_w)

cet_test(ValidHandleTester HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ValidHandleTester.fcl
  DATAFILES fcl/ValidHandleTester.fcl
  REQUIRED_FILES "../BareString_w.d/out.root"
  TEST_PROPERTIES DEPENDS BareString_w)

cet_test(issue_0926a HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c issue_0926a.fcl --rethrow-all
  DATAFILES fcl/issue_0926a.fcl fcl/messageDefaults.fcl
  )

cet_test(issue_0926b HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c issue_0926b.fcl --rethrow-all -s
  ../issue_0926a.d/issue_0926a.root
  DATAFILES fcl/issue_0926b.fcl fcl/messageDefaults.fcl
  REQUIRED_FILES ../issue_0926a.d/issue_0926a.root
  TEST_PROPERTIES DEPENDS issue_0926a
  )

foreach(copynum 1 2 3)
  cet_test(issue_0926b${copynum} HANDBUILT
    TEST_WORKDIR issue_0926b.d
    DIRTY_WORKDIR # Shouldn't clean up since we're sharing.
    TEST_EXEC cp
    REQUIRED_FILES issue_0926b.root
    TEST_ARGS -vf issue_0926b.root issue_0926b${copynum}.root
    TEST_PROPERTIES DEPENDS issue_0926b
    )
endforeach()

cet_test(issue_0926c_single HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c issue_0926c.fcl --rethrow-all -s ../issue_0926b.d/issue_0926b.root
  DATAFILES fcl/issue_0926c.fcl fcl/messageDefaults.fcl
  REQUIRED_FILES ../issue_0926b.d/issue_0926b.root
  TEST_PROPERTIES DEPENDS issue_0926b
  )

cet_test(issue_0926c_dupes HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c issue_0926c.fcl --rethrow-all -s
  ../issue_0926b.d/issue_0926b.root
  ../issue_0926b.d/issue_0926b1.root
  ../issue_0926b.d/issue_0926b2.root
  ../issue_0926b.d/issue_0926b3.root
  DATAFILES fcl/issue_0926c.fcl fcl/messageDefaults.fcl
  REQUIRED_FILES
  ../issue_0926b.d/issue_0926b.root
  ../issue_0926b.d/issue_0926b1.root
  ../issue_0926b.d/issue_0926b2.root
  ../issue_0926b.d/issue_0926b3.root
  )

set_tests_properties(issue_0926c_dupes PROPERTIES
  DEPENDS "issue_0926b;issue_0926b1;issue_0926b2;issue_0926b3"
  )

cet_test(outputCommand_t.sh PREBUILT
  DATAFILES
  fcl/outputCommand_w.fcl
  fcl/outputCommand_r.fcl
  fcl/messageDefaults.fcl)

simple_plugin(NonPersistableProducer "module" NO_INSTALL)
cet_test(NonPersistableProducts_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c non_persistable_products_t.fcl -n 1
  DATAFILES fcl/non_persistable_products_t.fcl)

cet_test(test_failingProducer_t.sh PREBUILT
  DATAFILES
  fcl/test_failingProducer_w.fcl
  fcl/test_failingProducer_r.fcl
  fcl/messageDefaults.fcl)

cet_test(rename-histfile_t_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c rename-histfile_t.fcl
  DATAFILES fcl/rename-histfile_t.fcl)

cet_test(rename-histfile_t_check HANDBUILT
  TEST_EXEC ls
  TEST_ARGS -l ../rename-histfile_t_w.d/test_1_0_2_8.root
  TEST_PROPERTIES DEPENDS rename-histfile_t_w)

simple_plugin(Metadata "plugin" NO_INSTALL
              art_Framework_Core
              art_Utilities
              cetlib_except
              fhiclcpp
              MF_MessageLogger
              Boost::filesystem)
cet_test(Metadata_plugin_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c Metadata_plugin_t.fcl
  DATAFILES fcl/Metadata_plugin_t.fcl)

cet_test(Metadata_plugin_t2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c Metadata_plugin_t2.fcl
  DATAFILES fcl/Metadata_plugin_t2.fcl)

cet_test(Metadata_verify_hr HANDBUILT
  TEST_EXEC sam_metadata_dumper
  TEST_ARGS -H -s ../Metadata_plugin_t.d/out.root
  TEST_PROPERTIES DEPENDS Metadata_plugin_t
  REF ${CMAKE_CURRENT_SOURCE_DIR}/Metadata_verify_hr-ref.txt)

cet_test(Metadata_verify_JSON HANDBUILT
  TEST_EXEC sam_metadata_dumper
  TEST_ARGS -s ../Metadata_plugin_t.d/out.root
  TEST_PROPERTIES DEPENDS Metadata_plugin_t
  REF ${CMAKE_CURRENT_SOURCE_DIR}/Metadata_verify_JSON-ref.txt)

cet_test(Assns_w HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "Assns_w.fcl"
  DATAFILES fcl/Assns_w.fcl)

foreach(num 1 2 3)
  cet_test(Assns_r${num} HANDBUILT
    TEST_EXEC art_ut
    TEST_ARGS -- -c "Assns_r${num}.fcl"
    REQUIRED_FILES ../Assns_w.d/out.root
    DATAFILES
    fcl/Assns_r_inc.fcl
    fcl/Assns_r${num}.fcl
    TEST_PROPERTIES DEPENDS Assns_w)
endforeach()

cet_test(AssnsFastClone_w HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "AssnsFastClone_w.fcl"
  DATAFILES fcl/AssnsFastClone_w.fcl
  TEST_PROPERTIES DEPENDS Assns_w)

foreach(mode M S P)
  cet_test(config_dumper_${mode}_t HANDBUILT
    TEST_EXEC config_dumper
    TEST_ARGS -${mode} ../Assns_w.d/out.root
    REF "${CMAKE_CURRENT_SOURCE_DIR}/config_dumper_${mode}_t-ref.txt"
    TEST_PROPERTIES
    DEPENDS Assns_w
    REQUIRED_FILES ../Assns_w.d/out.root)
endforeach()

cet_test(EventSelectorFromFile_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config EventSelectorFromFile_w.fcl -j 4
  DATAFILES fcl/EventSelectorFromFile_w.fcl)

set(NUM_THREADS 1 2 4 8)

cet_test(EventSelectorFromFile_r1 HANDBUILT
  TEST_EXEC art
  PARG_J -j ${NUM_THREADS}
  TEST_ARGS --config EventSelectorFromFile_r1.fcl
  DATAFILES fcl/EventSelectorFromFile_r1.fcl
  TEST_PROPERTIES DEPENDS EventSelectorFromFile_w)

cet_test(EventSelectorFromFile_r2 HANDBUILT
  TEST_EXEC art
  PARG_J -j ${NUM_THREADS}
  TEST_ARGS --config EventSelectorFromFile_r2.fcl
  DATAFILES fcl/EventSelectorFromFile_r2.fcl
  TEST_PROPERTIES DEPENDS EventSelectorFromFile_w)

cet_test(SelectEventsFromPreviousProcess_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS
  --config SelectEventsFromPreviousProcess_w.fcl
  -j 4
  -n 10
  -o out.root
  DATAFILES fcl/SelectEventsFromPreviousProcess_w.fcl)

cet_test(SelectEventsFromPreviousProcess_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config SelectEventsFromPreviousProcess_t.fcl
  -s ../SelectEventsFromPreviousProcess_w.d/out.root
  -j 4
  DATAFILES fcl/SelectEventsFromPreviousProcess_t.fcl
  REQUIRED_FILES "../SelectEventsFromPreviousProcess_w.d/out.root"
  TEST_PROPERTIES DEPENDS SelectEventsFromPreviousProcess_w)

cet_test(SelectEventsFromPreviousProcess_EventList_t HANDBUILT
  TEST_EXEC file_info_dumper
  TEST_ARGS --event-list ../SelectEventsFromPreviousProcess_t.d/out.root
  REF "${CMAKE_CURRENT_SOURCE_DIR}/SelectEventsFromPreviousProcess_t-ref.txt"
  REQUIRED_FILES "../SelectEventsFromPreviousProcess_t.d/out.root"
  TEST_PROPERTIES DEPENDS SelectEventsFromPreviousProcess_t)

add_executable(test_dropAllEventsSubruns_verify test_dropAllEventsSubruns_verify.cxx)
target_link_libraries(test_dropAllEventsSubruns_verify ROOT::Core ROOT::RIO ROOT::Tree)

#test_dropAllEventsSubruns_verify.cxx
cet_test(test_dropAllEventsSubruns_t.sh PREBUILT
  DATAFILES
  fcl/dropAllEvents_t.fcl
  fcl/dropAllEventsSubruns_t1.fcl
  fcl/dropAllEventsSubruns_t2.fcl
  fcl/dropAllEvents_r_t.fcl
  fcl/dropAllEventsSubruns_r_t1.fcl
  fcl/dropAllEventsSubruns_r_t2.fcl)

set(testOutputRanges_dropAllEventsSubruns_FILES
  "../test_dropAllEventsSubruns_t.sh.d/out_dropAllEvents.root"
  "../test_dropAllEventsSubruns_t.sh.d/out_dropAllEventsSubruns1.root"
  "../test_dropAllEventsSubruns_t.sh.d/out_dropAllEventsSubruns2.root"
  "../test_dropAllEventsSubruns_t.sh.d/out_dropAllEvents_r.root"
  "../test_dropAllEventsSubruns_t.sh.d/out_dropAllEventsSubruns1_r.root"
  "../test_dropAllEventsSubruns_t.sh.d/out_dropAllEventsSubruns2_r.root")

cet_test(testOutputRanges_dropAllEventsSubruns HANDBUILT
  TEST_EXEC file_info_dumper
  TEST_ARGS
  --range-of-validity
  --file-index ${testOutputRanges_dropAllEventsSubruns_FILES}
  REQUIRED_FILES ${testOutputRanges_dropAllEventsSubruns_FILES}
  REF "${CMAKE_CURRENT_SOURCE_DIR}/testOutputRanges_dropAllEventsSubRuns-ref.txt"
  TEST_PROPERTIES DEPENDS test_dropAllEventsSubruns_t.sh)

cet_test(test_runSubRunNoPut_t.sh PREBUILT
  DATAFILES fcl/runSubRunNoPut_t.fcl)

cet_test(simple_01_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config simple_01_t.fcl
  DATAFILES fcl/simple_01_t.fcl)

cet_test(simple_01_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config simple_01_r.fcl -s ../simple_01_t.d/out.root
  DATAFILES fcl/simple_01_r.fcl
  REQUIRED_FILES ../simple_01_t.d/out.root
  TEST_PROPERTIES DEPENDS simple_01_t)

cet_test(test_view_01_t.sh PREBUILT
  DATAFILES
  fcl/test_view_01a.fcl
  fcl/test_view_01b.fcl
  fcl/simple_01_r.fcl
  fcl/messageDefaults.fcl)

cet_test(IntProducer_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS
  -c IntProducer_t.fcl
  -o out.root
  --process-name MakeInt
  DATAFILES fcl/IntProducer_t.fcl)

add_subdirectory(kitchen-sink)

cet_test(SimpleDerived_01_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_simplederived_01a.fcl --errorOnMissingConsumes
  DATAFILES fcl/test_simplederived_01a.fcl)

cet_test(SimpleDerived_01_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_simplederived_01b.fcl
  DATAFILES fcl/test_simplederived_01b.fcl
  REQUIRED_FILES "../SimpleDerived_01_w.d/out.root"
  TEST_PROPERTIES DEPENDS SimpleDerived_01_w)

cet_test(SimpleDerived_02_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c SimpleDerived_02.fcl
  DATAFILES fcl/SimpleDerived_02.fcl
  REQUIRED_FILES "../SimpleDerived_01_w.d/out.root"
  TEST_PROPERTIES DEPENDS SimpleDerived_01_w)

cet_test(CmdLineTmpDir_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c cmdLineTmpDir.fcl --tmpDir=tmp --debug-config=CmdLineTmpDir_w.out
  DATAFILES fcl/cmdLineTmpDir.fcl)

cet_test(CmdLineTmpDir_t HANDBUILT
  TEST_EXEC diff
  TEST_ARGS cmdLineTmpDir_t-ref.txt "../CmdLineTmpDir_w.d/CmdLineTmpDir_w.out"
  DATAFILES cmdLineTmpDir_t-ref.txt
  TEST_PROPERTIES DEPENDS CmdLineTmpDir_w)

simple_plugin(EmptyPSetRegistryChecker "module" NO_INSTALL USE_BOOST_UNIT hep_concurrency)

cet_test(EmptyPSetRegistryChecker_w1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c EmptyPSetRegistryChecker_w1.fcl
  DATAFILES fcl/EmptyPSetRegistryChecker_w1.fcl)

cet_test(EmptyPSetRegistryChecker_r1 HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c EmptyPSetRegistryChecker_r1.fcl
  DATAFILES fcl/EmptyPSetRegistryChecker_r1.fcl
  TEST_PROPERTIES DEPENDS EmptyPSetRegistryChecker_w1)

cet_test(EmptyPSetRegistryChecker_w2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c EmptyPSetRegistryChecker_w2.fcl
  DATAFILES fcl/EmptyPSetRegistryChecker_w1.fcl
  fcl/EmptyPSetRegistryChecker_w2.fcl)

cet_test(EmptyPSetRegistryChecker_r2 HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c EmptyPSetRegistryChecker_r2.fcl
  DATAFILES fcl/EmptyPSetRegistryChecker_r1.fcl
  fcl/EmptyPSetRegistryChecker_r2.fcl
  TEST_PROPERTIES DEPENDS EmptyPSetRegistryChecker_w2)

simple_plugin(FindManySpeedTestProducer "module" NO_INSTALL)
simple_plugin(FindManySpeedTestAnalyzer "module" NO_INSTALL)

add_subdirectory(results-products)

cet_test(test_compressed_simple_01_t.sh PREBUILT
  DATAFILES
  fcl/test_compressed_simple_01.fcl
  fcl/test_compressed_simple_01r.fcl
  fcl/messageDefaults.fcl)

cet_test(MultiScheduleTransitions_t HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c MultiScheduleTransitions_t.fcl --nschedules 2
  DATAFILES fcl/MultiScheduleTransitions_t.fcl)

cet_test(ProductsWithEmptyEventCustomSource_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c GeneratorSource_t.fcl
  DATAFILES fcl/GeneratorSource_t.fcl)
