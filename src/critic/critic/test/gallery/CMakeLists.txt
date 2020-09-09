# Set the test environment so that pyROOT and interactive ROOT can find all necessary headers
#
cet_test_env(ROOT_INCLUDE_PATH=${PROJECT_SOURCE_DIR}:${CMAKE_INSTALL_PREFIX}/${${product}_inc_dir}:/usr/local/include)

set(gallery_tests_libraries
  critic_test_CriticTestObjects
  gallery
  canvas
  fhiclcpp
  cetlib
  cetlib_except
  ROOT::RIO
  ROOT::Core
  )

########################################################################
#
# This test suite generates a considerable number of files, in many
# cases using one file as the basis for the generation of the next.
# The dependencies are described below, showing which input file is used
# to generate a specific output file. The notation [ test_name ] is used
# to indicate that rather than going into the creation of a file, the
# specified inputs are used to run a test named "test_name".
#
# empty.root is an art/ROOT file with no events.
# [ make_empty_file_t ] -> test_gallery_empty.root
#
# test_gallery1.root -> test_gallery2.root
#
# test_gallery2.root -> test_gallery3.root
# test_gallery2.root -> test_gallery4.root
# test_gallery2.root -> test_gallery6.root
#
# test_gallery3.root + test_gallery4.root -> test_gallery5.root
# test_gallery6.root -> test_gallery7.root
# test_gallery7.root -> test_gallery8.root
#
# test_gallery5.root + test_gallery7.root + test_gallery8.root -> [ gallery_t ]
# test_gallery5.root + test_gallery7.root -> [ gallery_t.C ]
# test_gallery5.root + test_gallery7.root -> [ gallery_t.py ]
#
# out1.root -> [ findOne_1_t ]
# out1.root -> [ findOne_2_t ]
# out1.root -> [ findOne_3_t ]
#
#------------------------------------------------------------------------
#
# Make a hierarchy of input files in order to test gallery in C++,
# interpreted C++ and python.

cet_test(gallery_makeEmptyFile HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_gallery_makeEmptyFile.fcl
  DATAFILES
  fcl/test_gallery_makeEmptyFile.fcl
)

cet_test(gallery_makeInput1 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_gallery_makeInput1.fcl
  DATAFILES
  fcl/test_gallery_makeInput1.fcl
  )

# Uses output from gallery_makeInput1 and gallery_makeEmptyFile
cet_test(gallery_makeInput2 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_gallery_makeInput2.fcl ../gallery_makeInput1.d/test_gallery1.root
  DATAFILES
  fcl/test_gallery_makeInput2.fcl
  REQUIRED_FILES ../gallery_makeInput1.d/test_gallery1.root ../gallery_makeEmptyFile.d/test_gallery_empty.root
  )
set_tests_properties(gallery_makeInput2 PROPERTIES DEPENDS "gallery_makeEmptyFile;gallery_makeInput1")

# Uses output from gallery_makeInput2.
cet_test(gallery_makeInput3 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_gallery_makeInput3.fcl ../gallery_makeInput2.d/test_gallery2.root
  DATAFILES
  fcl/test_gallery_makeInput3.fcl
  REQUIRED_FILES ../gallery_makeInput2.d/test_gallery2.root
  TEST_PROPERTIES DEPENDS gallery_makeInput2
  )

# Uses output from gallery_makeInput2 with a differently-configured module.
cet_test(gallery_makeInput4 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_gallery_makeInput4.fcl ../gallery_makeInput2.d/test_gallery2.root
  DATAFILES
  fcl/test_gallery_makeInput4.fcl
  REQUIRED_FILES ../gallery_makeInput2.d/test_gallery2.root
  TEST_PROPERTIES DEPENDS gallery_makeInput2
  )

# Uses output from gallery_makeInput3 and gallery_makeInput4.
cet_test(gallery_makeInput5 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_gallery_makeInput5.fcl
  ../gallery_makeInput3.d/test_gallery3.root
  ../gallery_makeInput4.d/test_gallery4.root
  DATAFILES
  fcl/test_gallery_makeInput5.fcl
  REQUIRED_FILES ../gallery_makeInput3.d/test_gallery3.root
  ../gallery_makeInput4.d/test_gallery4.root
  )

set_tests_properties(gallery_makeInput5 PROPERTIES DEPENDS "gallery_makeInput3;gallery_makeInput4")

# Use gallery_makeInput2 to make an output incompatible with other files from the same ancestor.
cet_test(gallery_makeInput6 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_gallery_makeInput6.fcl ../gallery_makeInput2.d/test_gallery2.root
  DATAFILES
  fcl/test_gallery_makeInput6.fcl
  REQUIRED_FILES ../gallery_makeInput2.d/test_gallery2.root
  TEST_PROPERTIES DEPENDS gallery_makeInput2
  )

# Uses output from gallery_makeInput6
cet_test(gallery_makeInput7 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_gallery_makeInput7.fcl ../gallery_makeInput6.d/test_gallery6.root
  DATAFILES
  fcl/test_gallery_makeInput7.fcl
  REQUIRED_FILES ../gallery_makeInput6.d/test_gallery6.root
  TEST_PROPERTIES DEPENDS gallery_makeInput6
  )

# Uses output from gallery_makeInput7
cet_test(gallery_makeInput8 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c test_gallery_makeInput8.fcl ../gallery_makeInput7.d/test_gallery7.root
  DATAFILES
  fcl/test_gallery_makeInput8.fcl
  REQUIRED_FILES ../gallery_makeInput7.d/test_gallery7.root
  TEST_PROPERTIES DEPENDS gallery_makeInput7
  )

# Use gallery in compiled C++ to read output from gallery_makeInput5,
# gallery_makeInput7 and gallery_makeInput8.
cet_test(gallery_t
  LIBRARIES ${gallery_tests_libraries}
  REQUIRED_FILES ../gallery_makeInput5.d/test_gallery5.root
  ../gallery_makeInput7.d/test_gallery7.root
  ../gallery_makeInput8.d/test_gallery8.root
  )
# Enable plugins to access symbols exported by the exec (CMake policy CMP0065).
set_property(TARGET gallery_t PROPERTY ENABLE_EXPORTS TRUE)
set_property(TEST gallery_t PROPERTY
  DEPENDS "gallery_makeInput5;gallery_makeInput7;gallery_makeInput8")

# Use gallery to read a single file that is empty.
cet_test(gallery_empty_file_t
  LIBRARIES ${gallery_tests_libraries}
  REQUIRED_FILES ../gallery_makeEmptyFile.d/test_gallery_empty.root
  TEST_PROPERTIES DEPENDS gallery_makeEmptyFile
  TEST_ARGS ../gallery_makeEmptyFile.d/test_gallery_empty.root
  )

# Test gallery random access i/o.
cet_test(gallery_random_io_t
  LIBRARIES ${gallery_tests_libraries}
  REQUIRED_FILES ../gallery_makeInput3.d/test_gallery3.root
  TEST_PROPERTIES DEPENDS gallery_makeInput3
  TEST_ARGS ../gallery_makeInput3.d/test_gallery3.root
  )


# Use gallery in interpreted C++ to read output from gallery_makeInput5
# and gallery_makeInput7.
cet_test(gallery_t.C HANDBUILT
  TEST_EXEC root
  TEST_ARGS -b -q -l gallery_t.C
  DATAFILES gallery_t.C
  REQUIRED_FILES ../gallery_makeInput5.d/test_gallery5.root
  ../gallery_makeInput7.d/test_gallery7.root
  )

set_tests_properties(gallery_t.C PROPERTIES
  DEPENDS "gallery_makeInput5;gallery_makeInput7")

#if($ENV{CET_SANITIZE_ADDRESS})
#  #message(STATUS "CET_SANITIZE_ADDRESS=$ENV{CET_SANITIZE_ADDRESS}")
#  if(NOT "$ENV{GCC_FQ_DIR}" STREQUAL "")
#    #message(STATUS "GCC_FQ_DIR=$ENV{GCC_FQ_DIR}")
#    find_file(ASAN_LIB libasan.so NO_DEFAULT_PATH PATHS $ENV{GCC_FQ_DIR}/lib64)
#    if(NOT ASAN_LIB STREQUAL "ASAN_LIB-NOTFOUND")
#      #message(STATUS "ASAN_LIB=${ASAN_LIB}")
#    endif()
#  endif()
#endif()

#if($ENV{CET_SANITIZE_THREAD})
#  #message(STATUS "CET_SANITIZE_THREAD=$ENV{CET_SANITIZE_THREAD}")
#  if(NOT "$ENV{GCC_FQ_DIR}" STREQUAL "")
#    #message(STATUS "GCC_FQ_DIR=$ENV{GCC_FQ_DIR}")
#    find_file(TSAN_LIB libtsan.so NO_DEFAULT_PATH PATHS $ENV{GCC_FQ_DIR}/lib64)
#    if(NOT TSAN_LIB STREQUAL "TSAN_LIB-NOTFOUND")
#      #message(STATUS "TSAN_LIB=${TSAN_LIB}")
#    endif()
#  endif()
#endif()

if($ENV{CET_SANITIZE_ADDRESS})
  if(NOT ASAN_LIB STREQUAL "ASAN_LIB-NOTFOUND")
  set_tests_properties(gallery_t.C PROPERTIES
    ENVIRONMENT "LD_PRELOAD=${ASAN_LIB}")
  endif()
endif()

if($ENV{CET_SANITIZE_THREAD})
  if(NOT TSAN_LIB STREQUAL "TSAN_LIB-NOTFOUND")
  set_tests_properties(gallery_t.C PROPERTIES
    ENVIRONMENT "LD_PRELOAD=${TSAN_LIB}")
  endif()
endif()

# Use gallery in python to read output from gallery_makeInput5 and
# gallery_makeInput7.
cet_test(gallery_t.py HANDBUILT
  TEST_EXEC python3
  TEST_ARGS gallery_t.py
  DATAFILES gallery_t.py
  REQUIRED_FILES ../gallery_makeInput5.d/test_gallery5.root
  ../gallery_makeInput7.d/test_gallery7.root
  )
set_tests_properties(gallery_t.py PROPERTIES
  DEPENDS "gallery_makeInput5;gallery_makeInput7")

if($ENV{CET_SANITIZE_ADDRESS})
  if(NOT ASAN_LIB STREQUAL "ASAN_LIB-NOTFOUND")
  set_tests_properties(gallery_t.py PROPERTIES
    ENVIRONMENT "LD_PRELOAD=${ASAN_LIB}")
  endif()
endif()

if($ENV{CET_SANITIZE_THREAD})
  if(NOT TSAN_LIB STREQUAL "TSAN_LIB-NOTFOUND")
  set_tests_properties(gallery_t.py PROPERTIES
    ENVIRONMENT "LD_PRELOAD=${TSAN_LIB}")
  endif()
endif()

cet_test(CriticAssns_w HANDBUILT
  TEST_EXEC art_ut
  TEST_ARGS -- -c "CriticAssns_w.fcl"
  DATAFILES
  fcl/CriticAssns_w.fcl
)

foreach (num 1 2 3)
  cet_test(findOne_${num}_t USE_BOOST_UNIT
    LIBRARIES ${gallery_tests_libraries}
    TEST_PROPERTIES DEPENDS CriticAssns_w
    REQUIRED_FILES ../CriticAssns_w.d/out1.root
    )
endforeach()

simple_plugin(GalleryTestProducer     "module"  NO_INSTALL art_Persistency_Provenance cetlib_except)
simple_plugin(GalleryPtrTestProducer  "module"  NO_INSTALL critic_test_CriticTestObjects art_Persistency_Provenance cetlib_except)
simple_plugin(GalleryAssnsProducer    "module"  NO_INSTALL critic_test_CriticTestObjects art_Persistency_Provenance cetlib_except)
simple_plugin(CriticFailEverything    "module"  NO_INSTALL)
