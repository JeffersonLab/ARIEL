if(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR
    CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
  cet_add_compiler_flags(CXX -Wno-error=range-loop-analysis)
endif()
art_dictionary(DICTIONARY_LIBRARIES canvas canvas_root_io)
