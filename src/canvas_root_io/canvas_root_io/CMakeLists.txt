include(CetMake)

# cmake will automatically order the library builds according to declared dependencies
add_subdirectory(Dictionaries)
add_subdirectory(Streamers)
add_subdirectory(Utilities)

find_package(Threads)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

cet_make(LIBRARY_NAME canvas_root_io
  SUBDIRS
  Streamers
  Utilities
  LIBRARIES
  MF_MessageLogger
  ${ROOT_CORE}
  ${TBB}
  canvas
  fhiclcpp
  cetlib
  cetlib_except
  ${CLHEP}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS}
  NO_DICTIONARY
  )

# Testing
add_subdirectory(test)
