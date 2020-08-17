# Tests to expose certain instances of high memory use.

art_dictionary(NO_INSTALL)

simple_plugin(HMSubRunProdProducer "module" NO_INSTALL)
simple_plugin(HMRunProdProducer "module" NO_INSTALL)
simple_plugin(TestRemoveCachedProduct "module" NO_INSTALL USE_BOOST_UNIT)

cet_test(HMSubRun_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config HMSubRun_w.fcl
  DATAFILES fcl/HMSubRun_w.fcl fcl/HMParameters.fcl
  OPTIONAL_GROUPS LONG)

cet_test(HMSubRun_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config HMSubRun_r.fcl
  DATAFILES fcl/HMSubRun_r.fcl fcl/HMParameters.fcl
  OPTIONAL_GROUPS LONG
  TEST_PROPERTIES DEPENDS HMSubRun_w)

cet_test(TestRemoveCachedProduct_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config TestRemoveCachedProduct_w.fcl
  DATAFILES fcl/TestRemoveCachedProduct_w.fcl)

cet_test(TestRemoveCachedProduct_r HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- --config TestRemoveCachedProduct_r.fcl
  DATAFILES fcl/TestRemoveCachedProduct_r.fcl
  TEST_PROPERTIES DEPENDS TestRemoveCachedProduct_w)
