simple_plugin(AssnsIterProducer1     "module" NO_INSTALL)
simple_plugin(AssnsIterProducer2     "module" NO_INSTALL)
simple_plugin(ConstAssnsIterAnalyzer "module" NO_INSTALL)

cet_test(assns_const_iter HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config assns_const_iter.fcl
  DATAFILES assns_const_iter.fcl)
