set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR})

set(standard_libraries
  canvas
  ${CMAKE_DL_LIBS})

cet_test(EventID_t USE_BOOST_UNIT
  LIBRARIES canvas)

file(GLOB cppunit_files *.cppunit.cc)
foreach(cppunit_source ${cppunit_files})
  get_filename_component(test_name ${cppunit_source} NAME_WE )
  cet_test(${test_name} SOURCES testRunner.cpp ${cppunit_source}
    LIBRARIES ${CPPUNIT} ${standard_libraries})
endforeach()

foreach(test_source ModuleDescription_t.cpp ProcessConfiguration_t.cpp ProcessHistory_t.cpp)
  get_filename_component(test_name ${test_source} NAME_WE )
  cet_test(${test_name} SOURCES ${test_source} LIBRARIES ${standard_libraries})
endforeach()

cet_test(EventRange_t USE_BOOST_UNIT
  LIBRARIES
  canvas)

cet_test(RangeSet_t USE_BOOST_UNIT
  LIBRARIES
  canvas)

cet_test(ParentageRegistry_t USE_BOOST_UNIT LIBRARIES canvas)
