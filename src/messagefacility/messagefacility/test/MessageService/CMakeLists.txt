# ======================================================================
#
# Testing script
#
# ======================================================================

# Get all FHiCL files
file(GLOB testConfigFiles ${CMAKE_CURRENT_SOURCE_DIR}/fcl/*.fcl )
list(REMOVE_ITEM testConfigFiles ${CMAKE_CURRENT_SOURCE_DIR}/fcl/statistics_duplicateDest.fcl)

foreach(filename ${testConfigFiles})

  # trim directory prefixes and .fcl suffix
  string(REGEX REPLACE ".*/fcl/(.*)\\.fcl" "\\1" testname ${filename})

  cet_test(${testname}_t HANDBUILT
    TEST_EXEC ELdestinationTester
    TEST_ARGS -c ${testname}.fcl
    DATAFILES
    fcl/${testname}.fcl
    )

endforeach()

cet_test(statistics_duplicateDest_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS -c statistics_duplicateDest.fcl
  DATAFILES
  fcl/statistics_duplicateDest.fcl
  TEST_PROPERTIES WILL_FAIL TRUE
  )
