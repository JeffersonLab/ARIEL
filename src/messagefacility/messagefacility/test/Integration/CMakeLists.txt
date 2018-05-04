# ======================================================================
#
# Testing script
#
# ======================================================================
include(BasicPlugin)

cet_make_exec(ELdestinationTester
  SOURCE
    ELdestinationTester.cc
  LIBRARIES
    MF_MessageLogger
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

cet_test(Presence_init_t
  LIBRARIES
  MF_MessageLogger
  fhiclcpp
)

cet_test(WildcardDebug_t
  LIBRARIES
    MF_MessageLogger
    fhiclcpp
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
  DATAFILES
    fcl/WildcardDebug_t.fcl
  TEST_ARGS
    -c WildcardDebug_t.fcl
  REF
    ${CMAKE_CURRENT_SOURCE_DIR}/WildcardDebug_t-ref.txt
)

cet_make_exec(mfexample
  LIBRARIES
  MF_MessageLogger
  fhiclcpp
  pthread
  )

basic_plugin(Issue17457TestDestination mfPlugin
  MF_MessageLogger fhiclcpp ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY}
  NO_INSTALL
  )

foreach (N 01 02) # 03 04 05)
  set(test_stem Issue17457_${N})
  cet_test(${test_stem}_t HANDBUILT
    TEST_EXEC ELdestinationTester
    TEST_ARGS -c ${test_stem}.fcl
    DATAFILES fcl/${test_stem}.fcl
    OUTPUT_FILTERS DEFAULT ${CMAKE_CURRENT_SOURCE_DIR}/Issue17457-filter
    REF ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.out
    ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.err
    )
endforeach()
