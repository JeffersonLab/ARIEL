####################################
# Symlinks for ease of use

# Check and make links if necessary.
foreach(script check_outputDir check_inputFiles_symlink )
  add_custom_target(run_${script} ALL
    ${script} ${CMAKE_BINARY_DIR})

  cet_script(${script} NO_INSTALL)
  add_dependencies(run_${script} ${script})
endforeach()
