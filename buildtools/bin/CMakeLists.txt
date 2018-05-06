install_scripts( LIST
  cet_report
  cmake-graphviz-filt
  code-dep-graph
  increment-version
  report_product_info
  report_setfw
  )

cet_script(filter-output filter-output-art-compat cet_exec_test)
cet_script(checkClassVersion)

INSTALL ( PROGRAMS make_bash_completions DESTINATION bin )
INSTALL ( FILES cmake_parse_deps.pm parse_deps.pm DESTINATION bin )
