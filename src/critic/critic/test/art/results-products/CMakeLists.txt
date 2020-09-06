simple_plugin(RPTest "plugin" NO_INSTALL
  art_Framework_Core
  art_Framework_Principal
  canvas
  cetlib_except
  art_Persistency_Provenance
  fhiclcpp
  ${Boost_FILESYSTEM_LIBRARY})

simple_plugin(RPTestReader "plugin" NO_INSTALL
  art_Framework_Core
  art_Framework_Principal
  canvas
  cetlib_except
  art_Persistency_Provenance
  fhiclcpp
  ${Boost_FILESYSTEM_LIBRARY})

cet_test(RPTest_conf_err_01 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c RPTest_conf_err_01.fcl
  DATAFILES fcl/RPTest_conf_err_01.fcl
  TEST_PROPERTIES PASS_REGULAR_EXPRESSION "o1\\.results\\.rpath\\[2\\] \\(rpx\\) is not defined in o1\\.results\\.producers\\.\n")

cet_test(RPTest_conf_err_02 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c RPTest_conf_err_02.fcl
  DATAFILES fcl/RPTest_conf_err_02.fcl
  TEST_PROPERTIES PASS_REGULAR_EXPRESSION "o1\\.results\\.rpath2\\[0\\] \\(rpr\\) is a duplicate: previously used in path rpath\\.\n")

cet_test(RPTest_conf_err_03 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c RPTest_conf_err_03.fcl
  DATAFILES fcl/RPTest_conf_err_03.fcl
  TEST_PROPERTIES PASS_REGULAR_EXPRESSION "Parameter set o1\\.results is non-empty, but does not contain\n")

cet_test(RPTest_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c RPTest_w.fcl
  DATAFILES fcl/RPTest_w.fcl)

cet_test(RPTest_w2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c RPTest_w2.fcl
  DATAFILES fcl/RPTest_w2.fcl fcl/RPTest_w.fcl)

cet_test(RPTest_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c RPTest_r.fcl
  DATAFILES fcl/RPTest_r.fcl)

cet_test(RPTest_r2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c RPTest_r2.fcl
  DATAFILES fcl/RPTest_r2.fcl)

set_tests_properties(RPTest_r RPTest_r2 PROPERTIES DEPENDS "RPTest_w;RPTest_w2")
