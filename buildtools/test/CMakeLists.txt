add_test(NAME CheckUpsVersion_t
  COMMAND ${CMAKE_COMMAND}
  -DCMAKE_MODULE_PATH=${PROJECT_SOURCE_DIR}/Modules
  -P ${CMAKE_CURRENT_SOURCE_DIR}/CheckUpsVersion_t.cmake
)
