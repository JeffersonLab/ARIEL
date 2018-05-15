install_scripts( LIST buildtool
  build_table
  build_version_file
  cet_report
  cmake-graphviz-filt
  code-dep-graph
  increment-version
  set_dev_products
  report_product_info
  report_setfw
  migrate-package
  )

cet_script(filter-output filter-output-art-compat filter-program-options cet_exec_test)
cet_script(checkClassVersion)

INSTALL ( PROGRAMS make_bash_completions DESTINATION ${product}/${version}/bin )
INSTALL ( FILES cet_test_functions.sh
  cmake_parse_deps.pm
  parse_deps.pm
  set_dep_check_report
  set_dev_bin
  set_dev_check
  set_dev_check_report
  set_dev_fhicl
  set_dev_fwsearch
  set_dev_lib
  set_dev_report
  DESTINATION ${product}/${version}/bin
  )
