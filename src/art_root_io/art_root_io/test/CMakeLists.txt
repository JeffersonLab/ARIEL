cet_enable_asserts()

include(CetTest)

cet_make_library(LIBRARY_NAME art_root_io_test_TH1Data
  SOURCE TH1Data.cc
  LIBRARIES ROOT::Hist ROOT::Core
  NO_INSTALL)
art_dictionary(DICTIONARY_LIBRARIES art_root_io_test_TH1Data NO_INSTALL)

add_subdirectory(bad-assns)
add_subdirectory(fastclonefail)

simple_plugin(TH1DataProducer "module" NO_INSTALL art_root_io_test_TH1Data ALLOW_UNDERSCORES)

cet_test(TH1Data_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c TH1Data_t.fcl
  DATAFILES fcl/TH1Data_t.fcl)

###############################################################
# Not integration tests
cet_test(GetFileFormatVersion SOURCES test_GetFileFormatVersion.cpp
  LIBRARIES art_root_io)

cet_test(RootErrorParser_t USE_CATCH_MAIN LIBRARIES art_root_io)
cet_test(RootErrorClassifier_t USE_CATCH_MAIN LIBRARIES art_root_io)
cet_test(RootOutputClosingCriteria_t USE_BOOST_UNIT LIBRARIES art_root_io)

add_subdirectory(RootDB)

# TFileService file-renaming
cet_test(TFileService_FileRenaming_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all -c /dev/null -o TFileService_FileRenaming.root)

cet_test(TFileService_FileRenaming_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS
  --rethrow-all
  -c /dev/null
  -s ../TFileService_FileRenaming_w.d/TFileService_FileRenaming.root
  -T "hists_for_%ifb.root"
  REQUIRED_FILES "../TFileService_FileRenaming_w.d/TFileService_FileRenaming.root"
  TEST_PROPERTIES DEPENDS TFileService_FileRenaming_w)

cet_test(TFileService_FileRenaming_verify.sh PREBUILT
  TEST_PROPERTIES DEPENDS TFileService_FileRenaming_t)

cet_test(FastCloningInitMessage_01_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all -c FastCloningInitMessage_01.fcl
  DATAFILES fcl/FastCloningInitMessage_01.fcl
  TEST_PROPERTIES PASS_REGULAR_EXPRESSION "\nInitial fast cloning configuration \\(from default\\): true\n")

cet_test(FastCloningInitMessage_02_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all -c FastCloningInitMessage_02.fcl
  DATAFILES
  fcl/FastCloningInitMessage_01.fcl
  fcl/FastCloningInitMessage_02.fcl
  TEST_PROPERTIES PASS_REGULAR_EXPRESSION "\nInitial fast cloning configuration \\(user-set\\): true\n")

cet_test(FastCloningInitMessage_03_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all -c FastCloningInitMessage_03.fcl
  DATAFILES
  fcl/FastCloningInitMessage_01.fcl
  fcl/FastCloningInitMessage_03.fcl
  TEST_PROPERTIES PASS_REGULAR_EXPRESSION "\nInitial fast cloning configuration \\(user-set\\): false\n")

simple_plugin(IntArrayAnalyzer "module" NO_INSTALL ALLOW_UNDERSCORES)
simple_plugin(IntArrayProducer "module" NO_INSTALL ALLOW_UNDERSCORES)

cet_test(PersistStdArrays_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all -c persistStdArrays_w.fcl -o out.root
  DATAFILES fcl/persistStdArrays_w.fcl)

cet_test(PersistStdArrays_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all -c persistStdArrays_r.fcl -s ../PersistStdArrays_w.d/out.root
  DATAFILES fcl/persistStdArrays_r.fcl
  REQUIRED_FILES "../PersistStdArrays_w.d/out.root"
  TEST_PROPERTIES DEPENDS PersistStdArrays_w)

simple_plugin(BitsetAnalyzer "module"  NO_INSTALL ALLOW_UNDERSCORES)
simple_plugin(BitsetProducer "module"  NO_INSTALL ALLOW_UNDERSCORES)

cet_test(PersistStdBitsets_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all -c persistStdBitsets_w.fcl -o out.root
  DATAFILES
  fcl/persistStdBitsets_w.fcl)

cet_test(PersistStdBitsets_r HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all -c persistStdBitsets_r.fcl -s ../PersistStdBitsets_w.d/out.root
  DATAFILES
  fcl/persistStdBitsets_r.fcl
  TEST_PROPERTIES DEPENDS PersistStdBitsets_w)

cet_test(PostCloseFileRename_Integration HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c PostCloseFileRename_t.fcl
  DATAFILES fcl/PostCloseFileRename_t.fcl)

simple_plugin(TestTFileService "module" NO_INSTALL
  art_root_io_TFileService_service ROOT::Hist ROOT::Graf ALLOW_UNDERSCORES)

simple_plugin(TestTFileServiceNoRegister "module" NO_INSTALL
  art_root_io_TFileService_service ALLOW_UNDERSCORES)

foreach(NUM RANGE 1 5)
  cet_test(TFileService_t_0${NUM} HANDBUILT
    TEST_EXEC art
    TEST_ARGS --rethrow-all --config TFileService_t_0${NUM}.fcl
    DATAFILES fcl/TFileService_t_0${NUM}.fcl)
endforeach()

# Test multiple modules on a path -- use same configuration as 01,
# with the addition of adding an art/ROOT output file.
cet_test(TFileService_t_06 HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all --config TFileService_t_01.fcl
  -o art_root_output.root
  DATAFILES fcl/TFileService_t_01.fcl)

# Test that TFileService file-switching is not allowed for more than
# one thread.
cet_test(TFileService_t_07 HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all --config TFileService_t_01.fcl -j2
  DATAFILES fcl/TFileService_t_01.fcl
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION "can be used only when 1 thread and 1 schedule are specified.")

cet_test(TFileService_t_08 HANDBUILT
  TEST_EXEC art
  TEST_ARGS --rethrow-all --config TFileService_t_08.fcl
  DATAFILES fcl/TFileService_t_08.fcl)

cet_test(TFileService_r_08 SOURCE TFileService_r_08.cxx
  LIBRARIES ROOT::Core ROOT::RIO ROOT::Graf ROOT::Hist
  REQUIRED_FILES ../TFileService_t_08.d/tfile_output.root
  TEST_PROPERTIES DEPENDS TFileService_t_08)

add_executable(TestTFileService_r TestTFileService_r.cxx)
target_link_libraries(TestTFileService_r ROOT::Core ROOT::RIO ROOT::Hist)

foreach(NUM 1 5 6) # N.B. No 'RANGE' for these two tests
  cet_test(TFileService_r_0${NUM} HANDBUILT
    TEST_EXEC TestTFileService_r
    TEST_ARGS TFileService_r_0${NUM}_input.txt
    DATAFILES TFileService_r_0${NUM}_input.txt
    TEST_PROPERTIES DEPENDS TFileService_t_0${NUM})
endforeach()

cet_test(CheckFileName_default_t1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config check_filename_default_t1.fcl
  DATAFILES fcl/check_filename_default_t1.fcl)

cet_test(CheckFileName_default_t2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config check_filename_default_t2.fcl
  DATAFILES fcl/check_filename_default_t2.fcl
  TEST_PROPERTIES PASS_REGULAR_EXPRESSION "Please change your file name to include the '%#' pattern")

cet_test(CheckFileName_required_t HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config check_filename_required_t.fcl
  DATAFILES fcl/check_filename_required_t.fcl
  TEST_PROPERTIES PASS_REGULAR_EXPRESSION "Please change your file name to include the '%#' pattern")

cet_test(CheckFileName_disabled_w HANDBUILT
  TEST_EXEC art
  TEST_ARGS --config check_filename_disabled_w.fcl -n 2
  DATAFILES fcl/check_filename_disabled_w.fcl)

cet_test(CheckFileName_disabled_t1 PREBUILT
  TEST_PROPERTIES DEPENDS CheckFileName_disabled_w)

cet_test(CheckFileName_disabled_t2 PREBUILT
  TEST_PROPERTIES DEPENDS CheckFileName_disabled_t1)
