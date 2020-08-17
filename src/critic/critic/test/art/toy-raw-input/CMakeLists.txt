art_make_library(
  LIBRARY_NAME critic_test_art_toy_input
  SOURCE ToySource.cc
  LIBRARIES
  art_Framework_Core
  art_Framework_IO_Sources
  art_Framework_Principal
  art_Persistency_Provenance
  fhiclcpp
  canvas
  cetlib_except
  NO_INSTALL)

simple_plugin(ToyRawFileInput   "source" USE_BOOST_UNIT NO_INSTALL critic_test_art_toy_input)
simple_plugin(ToyRawInput       "source" USE_BOOST_UNIT NO_INSTALL critic_test_art_toy_input)
simple_plugin(ToyRawInputTester "module" NO_INSTALL)

cet_test(ToyRawInput_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyRawInput_w.fcl
  DATAFILES
  fcl/ToyRawInput_w.fcl
  fcl/ToyRawInput_wr_f1.fcl
  fcl/ToyRawInput_common.fcl)

cet_test(ToyRawInput_r1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyRawInput_r1.fcl
  DATAFILES
  fcl/ToyRawInput_r1.fcl
  REQUIRED_FILES "../ToyRawInput_w.d/out.root"
  TEST_PROPERTIES DEPENDS ToyRawInput_w)

cet_test(ToyRawInput_r2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyRawInput_r2.fcl
  DATAFILES
  fcl/ToyRawInput_r2.fcl
  fcl/ToyRawInput_r1.fcl
  REQUIRED_FILES "../ToyRawInput_w.d/out.root"
  TEST_PROPERTIES DEPENDS ToyRawInput_w)

cet_test(ToyRawInput_r3 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyRawInput_r3.fcl
  DATAFILES
  fcl/ToyRawInput_r3.fcl
  fcl/ToyRawInput_r1.fcl
  REQUIRED_FILES "../ToyRawInput_w.d/out.root"
  TEST_PROPERTIES DEPENDS ToyRawInput_w)

# Attempts to create SubRunPrincipal without a valid RunPrincipal.
foreach(num 02 11)
  cet_test(ToyRawInput_t_${num} HANDBUILT
    TEST_EXEC art
    TEST_ARGS -c ToyRawInput_${num}.fcl
    DATAFILES
    fcl/ToyRawInput_${num}.fcl
    fcl/ToyRawInput_common.fcl
    TEST_PROPERTIES
    PASS_REGULAR_EXPRESSION "Neither inR nor outR are set")
endforeach()

# Attempts to create EventPrincipal without a valid SubRunPrincipal.
foreach(num 03 04 12)
  cet_test(ToyRawInput_t_${num} HANDBUILT
    TEST_EXEC art
    TEST_ARGS -c ToyRawInput_${num}.fcl
    DATAFILES
    fcl/ToyRawInput_${num}.fcl
    fcl/ToyRawInput_common.fcl
    TEST_PROPERTIES
    PASS_REGULAR_EXPRESSION "Neither inSR nor outSR are set")
endforeach()

cet_test(ToyRawInput_t_01 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyRawInput_01.fcl
  DATAFILES fcl/ToyRawInput_01.fcl)

cet_test(ToyRawInput_OutputCheck_t_01 HANDBUILT
  TEST_EXEC file_info_dumper
  TEST_ARGS
  --range-of-validity
  --file-index
  "../ToyRawInput_t_01.d/out.root"
  REF "${CMAKE_CURRENT_SOURCE_DIR}/ToyRawInput_t_01-ref.txt"
  TEST_PROPERTIES
  DEPENDS ToyRawInput_t_01)

foreach(num 05 06 07 08 09 10)
  cet_test(ToyRawInput_t_${num} HANDBUILT
    TEST_EXEC art
    TEST_ARGS -c ToyRawInput_${num}.fcl
    DATAFILES
    fcl/ToyRawInput_${num}.fcl
    fcl/ToyRawInput_common.fcl)
endforeach()

SET_TESTS_PROPERTIES(ToyRawInput_t_05 PROPERTIES
  PASS_REGULAR_EXPRESSION
  "readNext returned a 'new' SubRun that was the same as the previous SubRun")

SET_TESTS_PROPERTIES(ToyRawInput_t_06 PROPERTIES
  PASS_REGULAR_EXPRESSION
  "readNext returned a 'new' Run that was the same as the previous Run")

SET_TESTS_PROPERTIES(ToyRawInput_t_07 PROPERTIES
  PASS_REGULAR_EXPRESSION
  "readNext returned a new Run and Event without a SubRun")

SET_TESTS_PROPERTIES(ToyRawInput_t_08 PROPERTIES
  PASS_REGULAR_EXPRESSION
  "readNext returned true but created no new data")

SET_TESTS_PROPERTIES(ToyRawInput_t_09 PROPERTIES
  PASS_REGULAR_EXPRESSION
  "readNext returned false but created new data")

SET_TESTS_PROPERTIES(ToyRawInput_t_10 PROPERTIES
  PASS_REGULAR_EXPRESSION
  "readNext returned a new Run with a SubRun from the wrong Run")

cet_test(ToyRawFileInput_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyRawFileInput_w.fcl
  DATAFILES
  fcl/ToyRawFileInput_data.fcl
  fcl/ToyRawFileInput_w.fcl
  fcl/ToyRawInput_w.fcl
  fcl/ToyRawInput_wr_f1.fcl
  fcl/ToyRawInput_common.fcl)

cet_test(ToyRawFileInput_r1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyRawFileInput_r1.fcl
  DATAFILES
  fcl/ToyRawFileInput_r1.fcl
  fcl/ToyRawInput_r1.fcl
  TEST_PROPERTIES DEPENDS ToyRawFileInput_w)

cet_test(ToyRawFileInput_r2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyRawFileInput_r2.fcl
  DATAFILES
  fcl/ToyRawFileInput_r2.fcl
  fcl/ToyRawInput_r2.fcl
  fcl/ToyRawInput_r1.fcl
  TEST_PROPERTIES DEPENDS ToyRawFileInput_w)

cet_test(ToyRawFileInput_r3 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c ToyRawFileInput_r3.fcl
  DATAFILES
  fcl/ToyRawFileInput_r3.fcl
  fcl/ToyRawInput_r3.fcl
  fcl/ToyRawInput_r1.fcl
  TEST_PROPERTIES DEPENDS ToyRawFileInput_w)
