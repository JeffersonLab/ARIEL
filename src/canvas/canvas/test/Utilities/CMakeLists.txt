# cet_test macro

set(default_canvas_test_libraries
  canvas
  cetlib
  MF_MessageLogger
  )

cet_test(Level_t)

cet_test(InputTag_t USE_BOOST_UNIT
  LIBRARIES ${default_canvas_test_libraries}
  )

cet_test(ParameterSet_get_artInputTag_t
  LIBRARIES ${default_canvas_test_libraries}
  TEST_ARGS "dummy"
  REF "${CMAKE_CURRENT_SOURCE_DIR}/ParameterSet_get_artInputTag_t-ref.txt"
  )

cet_test(FriendlyName_t USE_BOOST_UNIT
  LIBRARIES ${default_canvas_test_libraries}
  )

cet_test(TypeID_t USE_BOOST_UNIT
  LIBRARIES ${default_canvas_test_libraries}
  )

cet_test(ensurePointer_t USE_BOOST_UNIT
  LIBRARIES ${default_canvas_test_libraries}
  )

cet_test(uniform_type_name_test USE_BOOST_UNIT
  LIBRARIES
  canvas
  )

cet_make_exec(EventIDMatcher_t
  NO_INSTALL
  LIBRARIES
  canvas
  cetlib
)

cet_test(EventIDMatcher_test HANDBUILT
  TEST_EXEC EventIDMatcher_t
)
