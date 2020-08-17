cet_test(FileDumperOutput_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c FileDumperOutputTest_w.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "PROCESS NAME.*Total products \\(present, not present\\): 4 \\(4, 0\\)\\."
  DATAFILES fcl/FileDumperOutputTest_w.fcl)

cet_test(FileDumperOutput_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c FileDumperOutputTest_r.fcl
  DATAFILES fcl/FileDumperOutputTest_r.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "PROCESS NAME.*Total products \\(present, not present\\): 4 \\(3, 1\\)\\."
  DEPENDS FileDumperOutput_w)

cet_test(Ptrmv_w HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "Ptrmv_w.fcl"
  DATAFILES fcl/Ptrmv_w.fcl)

cet_test(Ptrmv_r HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "Ptrmv_r.fcl"
  DATAFILES fcl/Ptrmv_r.fcl
  TEST_PROPERTIES DEPENDS Ptrmv_w)

# Verify the ProvenanceChecker operation with a non-trivial event
# structure.
cet_test(ProvenanceChecker_t HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "ProvenanceChecker_t.fcl"
  DATAFILES
  fcl/Ptrmv_w.fcl
  fcl/ProvenanceChecker_t.fcl)

# Test the ProvenanceDumper module template (write and check).
cet_test(ProvenanceDumper_w HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "ProvenanceDumper_w.fcl"
  DATAFILES
  fcl/FileDumperOutputTest_w.fcl
  fcl/ProvenanceDumper_w.fcl)

# Test the ProvenanceDumper module template (read and check).
foreach(num 1 2 3 4)
  cet_test(ProvenanceDumper_r${num} HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "ProvenanceDumper_r${num}.fcl"
  REQUIRED_FILES "../ProvenanceDumper_w.d/out.root"
  DATAFILES
  fcl/FileDumperOutputTest_r.fcl
  fcl/ProvenanceDumper_r1.fcl
  fcl/ProvenanceDumper_r${num}.fcl # Should be ignored as dup. as appropriate.
  TEST_PROPERTIES DEPENDS ProvenanceDumper_w)
endforeach()

cet_test(DropDescendentProducts_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c DropDescendentProducts_t.fcl
  REQUIRED_FILES "../ProvenanceDumper_w.d/out.root"
  DATAFILES fcl/DropDescendentProducts_t.fcl
  TEST_PROPERTIES DEPENDS ProvenanceDumper_w)

# Use the file from Ptrmv_w to test DropOnInput and outputCommand drops.
#
# 1. Drop Ptr, keep base MapVector.
# 2, Keep Ptr, drop base MapVector.
# 3. Drop both.
# 4. Keep both.
foreach(num RANGE 1 4)
  # Test drop on input.
  cet_test(DropOnInput_t_0${num} HANDBUILT
    TEST_EXEC art_ut
    TEST_ARGS --
    -c "DropOnInput_t_0${num}.fcl"
    -s "../Ptrmv_w.d/out.root"
    --output "out.root"
    REQUIRED_FILES "../Ptrmv_w.d/out.root"
    DATAFILES
    fcl/DropOnInput_t_01.fcl
    fcl/DropOnInput_t_0${num}.fcl
    TEST_PROPERTIES DEPENDS Ptrmv_w)

  cet_test(DropOnInput_r_0${num} HANDBUILT
    TEST_EXEC art_ut
    TEST_ARGS --
    -c "DropOnInput_r_0${num}.fcl"
    -s "../DropOnInput_t_0${num}.d/out.root"
    REQUIRED_FILES  "../DropOnInput_t_0${num}.d/out.root"
    DATAFILES
    fcl/DropOnInput_t_01.fcl
    fcl/DropOnInput_t_0${num}.fcl
    fcl/DropOnInput_r_0${num}.fcl
    TEST_PROPERTIES DEPENDS DropOnInput_t_0${num})

  # Test drop on output with input file.
  cet_test(DropOnOutput_wA_0${num} HANDBUILT
    TEST_EXEC art_ut
    TEST_ARGS -- -c "DropOnOutput_wA_0${num}.fcl"
    DATAFILES
    fcl/DropOnOutput_wA_01.fcl
    fcl/DropOnOutput_wA_0${num}.fcl
    TEST_PROPERTIES DEPENDS Ptrmv_w)

  cet_test(DropOnOutput_rA_0${num} HANDBUILT
    TEST_EXEC art_ut
    TEST_ARGS --
    -c "DropOnOutput_r_0${num}.fcl"
    -s "../DropOnOutput_wA_0${num}.d/out.root"
    DATAFILES
    fcl/DropOnOutput_r_01.fcl
    fcl/DropOnOutput_r_0${num}.fcl
    REQUIRED_FILES "../DropOnOutput_wA_0${num}.d/out.root"
    TEST_PROPERTIES DEPENDS DropOnOutput_wA_0${num})

  # Test drop on output with in-job generated products.
  cet_test(DropOnOutput_wB_0${num} HANDBUILT
    TEST_EXEC art_ut
    TEST_ARGS -- -c "DropOnOutput_wB_0${num}.fcl"
    DATAFILES
    fcl/Ptrmv_w.fcl
    fcl/DropOnOutput_wB_01.fcl
    fcl/DropOnOutput_wB_0${num}.fcl)

  cet_test(DropOnOutput_rB_0${num} HANDBUILT
    TEST_EXEC art_ut
    TEST_ARGS --
    -c "DropOnOutput_r_0${num}.fcl"
    -s "../DropOnOutput_wB_0${num}.d/out.root"
    REQUIRED_FILES "../DropOnOutput_wB_0${num}.d/out.root"
    DATAFILES
    fcl/DropOnOutput_r_01.fcl
    fcl/DropOnOutput_r_0${num}.fcl
    TEST_PROPERTIES DEPENDS DropOnOutput_wB_0${num})
endforeach()

cet_test(DropOnOutput_IntProduct_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS
  -c IntReaderThenProducer_t.fcl
  -s ../../IntProducer_t.d/out.root
  --process-name ReadThenMakeInt
  DATAFILES fcl/IntReaderThenProducer_t.fcl
  TEST_PROPERTIES DEPENDS IntProducer_t)

# Test ProvenanceDumper for dropped products
# Check when an IntProduct has been dropped
cet_test(ProvenanceDumper_DroppedProducts_t1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c TestProvenanceDumper.fcl -s ../DropOnOutput_IntProduct_t.d/out.root
  DATAFILES fcl/TestProvenanceDumper.fcl
  REQUIRED_FILES "../DropOnOutput_IntProduct_t.d/out.root"
  TEST_PROPERTIES DEPENDS DropOnOutput_IntProduct_t)

# Test ProvenanceDumper for dropped products
# Check when an art::Ptr has been dropped
cet_test(ProvenanceDumper_DroppedProducts_t2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c TestProvenanceDumper.fcl -s ../DropOnOutput_wA_01.d/out.root
  DATAFILES fcl/TestProvenanceDumper.fcl
  TEST_PROPERTIES DEPENDS DropOnOutput_wA_01)

simple_plugin(TestReadImmediateControl "module" NO_INSTALL USE_BOOST_UNIT)

foreach(num 1 2 3 4 5)
  cet_test(ImmediateReadControl_${num} HANDBUILT
    TEST_EXEC art_ut
    TEST_ARGS -- -c "ImmediateReadControl_${num}.fcl"
    REQUIRED_FILES "../FileDumperOutput_w.d/out.root"
    DATAFILES
    fcl/FileDumperOutputTest_r.fcl
    fcl/ImmediateReadControl_1.fcl
    fcl/ImmediateReadControl_${num}.fcl
    TEST_PROPERTIES DEPENDS FileDumperOutputTest_w)
endforeach()

cet_test(PtrVectorFastClone_w HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "PtrVectorFastClone_w.fcl"
  DATAFILES fcl/PtrVectorFastClone_w.fcl
  TEST_PROPERTIES DEPENDS Ptrmv_w)
