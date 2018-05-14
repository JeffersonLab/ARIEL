include(CetMake)

# cmake will automatically order the library builds according to declared dependencies
add_subdirectory(Version)
add_subdirectory(Utilities)
add_subdirectory(Persistency)

find_package(Threads)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "^(Apple)?Clang$")
  set(NEED_ROOT_INTROSPECTION TRUE)
  find_ups_root()
  set (INTROSPECTION_LIBS ${ROOT_CORE})
endif()

cet_make(LIBRARY_NAME canvas
  SUBDIRS Version ${CMAKE_CURRENT_BINARY_DIR}/Version
  Utilities
  Utilities/detail
  Persistency/Provenance
  Persistency/Provenance/Compatibility
  Persistency/Provenance/detail
  Persistency/Common
  Persistency/Common/detail
  LIBRARIES
  MF_MessageLogger
  ${INTROSPECTION_LIBS}
  ${TBB}
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
