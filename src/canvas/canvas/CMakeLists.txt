include(CetMake)

# cmake will automatically order the library builds according to declared dependencies
add_subdirectory(Version)
add_subdirectory(Utilities)
add_subdirectory(Persistency)

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
  $<$<PLATFORM_ID:Darwin>:c++abi>
  fhiclcpp
  cetlib
  cetlib_except
  hep_concurrency
  Boost::date_time
  ${CLHEP}
  ${CMAKE_DL_LIBS}
  NO_DICTIONARY)

# Testing
add_subdirectory(test)
