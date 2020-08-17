cet_test(EventID_t USE_BOOST_UNIT LIBRARIES canvas)

file(GLOB cppunit_files *.cppunit.cc)
foreach(cppunit_source ${cppunit_files})
  get_filename_component(test_name ${cppunit_source} NAME_WE)
  cet_test(${test_name} SOURCES testRunner.cpp ${cppunit_source}
    LIBRARIES ${CPPUNIT} canvas)
endforeach()

foreach(test_source ProcessConfiguration_t.cpp ProcessHistory_t.cpp)
  get_filename_component(test_name ${test_source} NAME_WE)
  cet_test(${test_name} SOURCES ${test_source} LIBRARIES canvas)
endforeach()

cet_test(EventRange_t USE_BOOST_UNIT LIBRARIES canvas)
cet_test(RangeSet_t USE_BOOST_UNIT LIBRARIES canvas)

find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

cet_test(ParentageRegistry_t USE_BOOST_UNIT LIBRARIES canvas cetlib Threads::Threads)
