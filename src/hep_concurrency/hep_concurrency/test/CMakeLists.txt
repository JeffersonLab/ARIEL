#include(CetTest)

#set(source_libraries hep_concurrency ${CPPUNIT} ${TBB} pthread)

#cet_make_exec(ThreadSafeOutputFileStream_t LIBRARIES pthread hep_concurrency)
#cet_test(runThreadSafeOutputFileStream_t.sh PREBUILT)

# Unfortunately, the SerialTaskQueue tests expose TBB assertion
# failures, that appear to be internal issues.  Until TBB fixes
# things, we are forced to disable the tests for the DEBUG build.

#string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
#if(NOT ${BTYPE_UC} STREQUAL "DEBUG")
  #cet_test(serialtaskqueuechain_t LIBRARIES ${source_libraries})
  #cet_test(serialtaskqueue_t LIBRARIES ${source_libraries})
#endif()

#cet_test(waitingtasklist_t LIBRARIES ${source_libraries})
