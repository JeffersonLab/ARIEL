vertex_tests(11)

foreach(WRAPPED "" "wrapped_")
  cet_test(${WRAPPED}vertex_v11_r_v10 HANDBUILT
    TEST_EXEC ${CMAKE_CURRENT_BINARY_DIR}/${WRAPPED}vertex_v11_reader
    TEST_ARGS ../../v10/${WRAPPED}vertex_v10_w.d/${WRAPPED}vertex_v10.root
    REQUIRED_FILES ../../v10/${WRAPPED}vertex_v10_w.d/${WRAPPED}vertex_v10.root
    REF ${CMAKE_CURRENT_SOURCE_DIR}/../vertex-ref.txt
    )

  set_property(TEST
    ${WRAPPED}vertex_v11_r_v10
    PROPERTY
    ENVIRONMENT LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_CURRENT_BINARY_DIR}
    )

  set_property(TEST
    ${WRAPPED}vertex_v11_r_v10
    APPEND
    PROPERTY
    ENVIRONMENT ROOT_INCLUDE_PATH=${v11_ROOT_INCLUDE_PATH}
    )

  set_property(TEST
    ${WRAPPED}vertex_v11_r_v10
    PROPERTY
    DEPENDS ${WRAPPED}vertex_v10_w
    )

  if (NOT ROOT_FIXED_GENERAL_IO_RULES)
    set_property(TEST
      ${WRAPPED}vertex_v11_r_v10
      PROPERTY
      WILL_FAIL TRUE
      )
  endif()
endforeach()
