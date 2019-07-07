####################################
# Symlinks for ease of use

set(PKG_SOURCE "${CMAKE_SOURCE_DIR}")

# Check and make links if necessary.
foreach(script check_inputFiles_symlink check_outputDir_symlink check_fcl_symlink )
  configure_file(${script}.in ${script} @ONLY)
  add_custom_target(run_${script} ALL
    "${CMAKE_CURRENT_BINARY_DIR}/${script}" "${CMAKE_BINARY_DIR}"
    )
endforeach()
