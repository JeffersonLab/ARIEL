set(LIB_NAME ${PROJECT_NAME})

cet_make_library(
  LIBRARY_NAME
    ${LIB_NAME}
  SOURCE
    RecursiveMutex.cc
    SerialTaskQueue.cc
    SerialTaskQueueChain.cc
    SharedResourceNames.cc
    ThreadSafeOutputFileStream.cc
    WaitingTask.cc
    WaitingTaskHolder.cc
    WaitingTaskList.cc
    tsan.cc
  LIBRARIES
    ${TBB}
)
target_include_directories(${LIB_NAME} PRIVATE ${TBB_INCLUDE_DIRS})

install_headers()
install_source()

add_subdirectory(test)

