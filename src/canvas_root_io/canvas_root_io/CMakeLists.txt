include(CetMake)

# cmake will automatically order the library builds according to declared dependencies
add_subdirectory(Dictionaries)
add_subdirectory(Streamers)
add_subdirectory(Utilities)

cet_make(LIBRARY_NAME canvas_root_io
  SUBDIRS
  Streamers
  Utilities
  LIBRARIES
  MF_MessageLogger
  ROOT::Core
  canvas
  hep_concurrency
  fhiclcpp
  cetlib
  cetlib_except
  ${CLHEP}
  NO_DICTIONARY
  )

# Testing
add_subdirectory(test)
