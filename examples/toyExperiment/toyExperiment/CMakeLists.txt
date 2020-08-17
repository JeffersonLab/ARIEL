set(TFILE_SERVICE_LIBRARIES ROOT::Core)
if (OLDER_THAN_v30200)
  list(APPEND TFILE_SERVICE_LIBRARIES
    art_Framework_Services_Optional_TFileService_service
    art_Framework_Services_Optional)
else()
  list(APPEND TFILE_SERVICE_LIBRARIES
    art_root_io_TFileService_service
    art_root_io_tfile_support)
endif()

add_subdirectory(Utilities)
add_subdirectory(RecoDataProducts)
add_subdirectory(MCDataProducts)
add_subdirectory(Reconstruction)
add_subdirectory(Simulations)
add_subdirectory(Analyzers)
add_subdirectory(DataProducts)
add_subdirectory(PDT)
add_subdirectory(Conditions)
add_subdirectory(Geometry)
add_subdirectory(HelloWorldScripts)
add_subdirectory(inputFiles)
add_subdirectory(databaseFiles)
add_subdirectory(fcl)
