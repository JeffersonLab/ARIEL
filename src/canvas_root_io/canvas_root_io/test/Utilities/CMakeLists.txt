art_dictionary(NO_DEFAULT_LIBRARIES DICTIONARY_LIBRARIES cetlib NO_INSTALL)

cet_make_exec(TypeNameBranchName_t
  NO_INSTALL
  LIBRARIES
  canvas
  cetlib
  cetlib_except
)

set(tnum 1)
foreach(types_file
    art_test_types.txt
    microboone_MC6.txt
    mu2e.txt
    lariat_r006037_sr0080.typetester.txt
    lbne_r001161_sr01_20150526T220054.art_typetester.txt
    )
  if (tnum LESS 10)
    set(tnum_text "0${tnum}")
  else()
    set(tnum_text "${tnum}")
  endif()
  cet_test(TypeNameBranchName_test_${tnum_text} HANDBUILT
    TEST_EXEC TypeNameBranchName_t
    TEST_ARGS ${CMAKE_CURRENT_SOURCE_DIR}/TypeNameBranchName_t/${types_file}
    )
  math(EXPR tnum "${tnum} + 1")
endforeach()

cet_test(RootClassMapping_t USE_BOOST_UNIT
  LIBRARIES
  canvas_root_io_test_Utilities_dict
  ${ROOT_CINTEX}
  ROOT::Tree
  ROOT::MathCore
  ROOT::RIO
  ROOT::Core
  ${ROOT_CINT}
  ${ROOT_REFLEX}
  ${CMAKE_DL_LIBS}
  # When ROOT is fixed WILL_FAIL should be removed and the code in ASSNS
  # (and associated ioread rules) should be fixed accordingly.
  TEST_PROPERTIES WILL_FAIL true
  )

cet_test(TypeTools_t USE_BOOST_UNIT
  LIBRARIES
  canvas_root_io
  ROOT::Tree
  ROOT::Hist
  ROOT::Matrix
  ROOT::MathCore
  ROOT::RIO
  ROOT::Core
  ${ROOT_CINT}
  ${ROOT_REFLEX}
  ${CMAKE_DL_LIBS}
  )

cet_test(TypeWithDict_t USE_BOOST_UNIT
  LIBRARIES
  canvas_root_io
  ROOT::Core
  )

cet_test(TWD_minimal_t
  LIBRARIES
  ROOT::Core
  )
