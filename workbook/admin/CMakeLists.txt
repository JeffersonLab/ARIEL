####################################
# Symlinks for ease of use

# Check and make links if necessary.
foreach(script check_inputFiles_symlink check_outputDir_symlink check_fcl_symlink )
  cet_script(${script} NO_INSTALL)
  add_custom_target(run_${script} ALL
    ${script} ${CMAKE_BINARY_DIR}
    DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${script}
    )
endforeach()
