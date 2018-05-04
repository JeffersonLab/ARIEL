configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/GetReleaseVersion.cc.in
  ${CMAKE_CURRENT_BINARY_DIR}/GetReleaseVersion.cc @ONLY )

install_headers()
install_source()
