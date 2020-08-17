# Write a data file containing stored products and stored random number
# states for subsequent tests.
cet_test(RandomNumberTestEventSave_w HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "RandomNumberTestEventSave_w.fcl"
  DATAFILES fcl/RandomNumberTestEventSave_w.fcl)

# Test the ability to restore random number states from data product.
cet_test(RandomNumberTestEventSave_r1 HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "RandomNumberTestEventSave_r.fcl"
  DATAFILES fcl/RandomNumberTestEventSave_r.fcl
  TEST_PROPERTIES DEPENDS RandomNumberTestEventSave_w)

# Test that we get numbers out of sync. when we do *not* restore the
# state from data product
cet_test(RandomNumberTestEventSave_r2 HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "RandomNumberTestEventSave_r2.fcl"
  DATAFILES
  fcl/RandomNumberTestEventSave_r.fcl
  fcl/RandomNumberTestEventSave_r2.fcl
  TEST_PROPERTIES WILL_FAIL true
  DEPENDS RandomNumberTestEventSave_w)

# Write the state file terminating normally after 9 events.
cet_test(RandomNumberTestFileSave_wA HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "RandomNumberTestFileSave_wA.fcl"
  DATAFILES
  fcl/RandomNumberTestEventSave_r.fcl
  fcl/RandomNumberTestFileSave_wA.fcl
  TEST_PROPERTIES DEPENDS RandomNumberTestEventSave_w)

# Read the normal-termination state file to process the 10th event.
cet_test(RandomNumberTestFileSave_rA HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "RandomNumberTestFileSave_rA.fcl"
  DATAFILES
  fcl/RandomNumberTestEventSave_r.fcl
  fcl/RandomNumberTestFileSave_rA.fcl
  TEST_PROPERTIES DEPENDS RandomNumberTestFileSave_wA)

# Write the state file terminating abnormally in event #10.
cet_test(RandomNumberTestFileSave_wB HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "RandomNumberTestFileSave_wB.fcl"
  DATAFILES
  fcl/RandomNumberTestEventSave_r.fcl
  fcl/RandomNumberTestFileSave_wB.fcl
  TEST_PROPERTIES WILL_FAIL true
  DEPENDS RandomNumberTestEventSave_w)

# Read the abnormal-termination state file to process the 10th event.
cet_test(RandomNumberTestFileSave_rB HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "RandomNumberTestFileSave_rB.fcl"
  DATAFILES
  fcl/RandomNumberTestEventSave_r.fcl
  fcl/RandomNumberTestFileSave_rB.fcl
  TEST_PROPERTIES DEPENDS RandomNumberTestFileSave_wB)
