configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_GetCanvasReleaseVersion.cpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/test_GetCanvasReleaseVersion.cpp @ONLY
  )

cet_test(GetCanvasReleaseVersion
  SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_GetCanvasReleaseVersion.cpp
  LIBRARIES canvas
  )

