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
                Boost::filesystem
                Boost::program_options)

cet_test(Presence_init_t LIBRARIES MF_MessageLogger fhiclcpp)

cet_test(WildcardDebug_t
         LIBRARIES MF_MessageLogger
                   fhiclcpp
                   Boost::filesystem
                   Boost::program_options
         DATAFILES fcl/WildcardDebug_t.fcl
         TEST_ARGS -c WildcardDebug_t.fcl
         REF ${CMAKE_CURRENT_SOURCE_DIR}/WildcardDebug_t-ref.txt)

basic_plugin(Issue17457TestDestination mfPlugin
             MF_MessageLogger
             fhiclcpp
             Boost::filesystem
             NO_INSTALL)

foreach (N 01 02) # 03 04 05)
  set(test_stem Issue17457_${N})
  cet_test(${test_stem}_t HANDBUILT
           TEST_EXEC ELdestinationTester
           TEST_ARGS -c ${test_stem}.fcl
           DATAFILES fcl/${test_stem}.fcl
           OUTPUT_FILTERS DEFAULT ${CMAKE_CURRENT_SOURCE_DIR}/Issue17457-filter
           REF ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.out
           ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.err)
endforeach()
