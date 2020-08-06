install_scripts( LIST
  cet_report
  cmake-graphviz-filt
  code-dep-graph
  report_product_info
  report_setfw
  )

cet_script(filter-output filter-output-art-compat filter-program-options cet_exec_test)
cet_script(checkClassVersion)

INSTALL ( PROGRAMS format-code make_bash_completions DESTINATION bin )
INSTALL ( FILES cet_test_functions.sh
  cmake_parse_deps.pm
  parse_deps.pm
  DESTINATION ${${product}_bin_dir} )
