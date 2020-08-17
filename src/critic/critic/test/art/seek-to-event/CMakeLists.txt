simple_plugin(SeekToEventClient "service" NO_INSTALL
  art_Framework_Core
  art_Framework_Principal
  art_root_io_RootInput_source
  art_root_io)

macro(seekToEventTest NEVENTS)
  set(SeekToEventPrefix "SeekToEvent_${NEVENTS}_events")
  set(SeekToEventFclPrefix "seekToEvent_${NEVENTS}_events")
  cet_test(${SeekToEventPrefix}_w HANDBUILT
    TEST_EXEC art
    TEST_ARGS -c /dev/null -n ${NEVENTS} -o out.root)

  cet_test(${SeekToEventPrefix}_t HANDBUILT
    TEST_EXEC art
    TEST_ARGS -c ${SeekToEventFclPrefix}.fcl ../${SeekToEventPrefix}_w.d/out.root -o out.root
    DATAFILES fcl/${SeekToEventFclPrefix}.fcl
    TEST_PROPERTIES DEPENDS ${SeekToEventPrefix}_w)

  cet_test(${SeekToEventPrefix}_verify HANDBUILT
    TEST_EXEC file_info_dumper
    TEST_ARGS --file-index ../${SeekToEventPrefix}_t.d/out.root
    REF "${CMAKE_CURRENT_SOURCE_DIR}/${SeekToEventPrefix}_verify-ref.txt"
    TEST_PROPERTIES DEPENDS ${SeekToEventPrefix}_t)
endmacro()

seekToEventTest(1)
seekToEventTest(4)

cet_test(SeekToEvent_3_events_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c seekToEvent_3_events_w.fcl -o out.root
  DATAFILES fcl/seekToEvent_3_events_w.fcl)

cet_test(SeekToEvent_3_events_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c seekToEvent_3_events_t.fcl ../SeekToEvent_3_events_w.d/out.root -o out.root
  DATAFILES fcl/seekToEvent_3_events_t.fcl
  TEST_PROPERTIES DEPENDS SeekToEvent_3_events_w)

cet_test(SeekToEvent_3_events_verify HANDBUILT
  TEST_EXEC file_info_dumper
  TEST_ARGS --file-index ../SeekToEvent_3_events_t.d/out.root
  REF "${CMAKE_CURRENT_SOURCE_DIR}/SeekToEvent_3_events_verify-ref.txt"
  TEST_PROPERTIES DEPENDS SeekToEvent_3_events_t)
