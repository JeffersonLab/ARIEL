configure_file(VersionInfo.pm.in ${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.pm @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.pm
  DESTINATION ${${product}_perllib}/CetSkel/cetskelgen)
