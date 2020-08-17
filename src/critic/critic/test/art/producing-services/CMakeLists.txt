simple_plugin(IntProducingService         "service" NO_INSTALL art_Framework_Principal art_Framework_Core art_Persistency_Provenance)
simple_plugin(LastDoubleProducingService  "service" NO_INSTALL art_Framework_Principal art_Framework_Core art_Persistency_Provenance)

cet_test(IntProducerRun_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS
  -c IntProducerRun_t.fcl
  -o out.root
  --process-name MakeInt
  DATAFILES
  fcl/IntProducerRun_t.fcl)

# Test product insertion via a service.  In addition, the product
# inserted is declared to be "Persistable::No" and is not persisted to
# an output file.
cet_test(ProducingService_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS
  -c ProducingService_w.fcl
  -s ../IntProducerRun_t.d/out.root
  DATAFILES fcl/ProducingService_w.fcl
  TEST_PROPERTIES DEPENDS IntProducerRun_t)

# Now verify that the "Persistable::No" product created above is not
# present in the input file.
cet_test(ProducingService_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS
  -c ProducingService_r.fcl
  -s ../ProducingService_w.d/out.root
  DATAFILES fcl/ProducingService_r.fcl
  TEST_PROPERTIES DEPENDS ProducingService_w)
