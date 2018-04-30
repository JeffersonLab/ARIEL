########################################################################
# cet_set_compiler_flags( [extra flags] ) 
#
#    sets the default compiler flags
#
# default gcc/g++ flags:
# DEBUG           -g
# RELEASE         -O3 -DNDEBUG
# MINSIZEREL      -Os -DNDEBUG
# RELWITHDEBINFO  -O2 -g
#
# CET flags
# (debug)   DEBUG      -g -O0
# (prof)    PROF       -O3 -g -DNDEBUG -fno-omit-frame-pointer
# (opt)     OPT        -O3 -g -DNDEBUG
#
# Plus the diagnostic option set indicated by the DIAG option.
#
# Optional arguments
#    DIAGS <diag-level>
#      This option may be CAVALIER, CAUTIOUS, VIGILANT or PARANOID.
#      Default is CAUTIOUS.
#
#    DWARF_STRICT
#      Instruct the compiler not to emit any debugging information more
#      advanced than that selected. This will prevent possible errors in
#      older debuggers, but may prevent certain C++11 constructs from
#      being debuggable in modern debuggers.
#
#    DWARF_VER <#>
#      Version of the DWARF standard to use for generating debugging
#      information. Default depends upon the compiler: GCC v4.8.0 and
#      above emit DWARF4 by default; earlier compilers emit DWARF2.
#
#    ENABLE_ASSERTS
#      Enable asserts regardless of debug level (default is to disable
#      asserts for PROF and OPT levels).
#
#    EXTRA_FLAGS (applied to both C and CXX) <flags>
#    EXTRA_C_FLAGS <flags>
#    EXTRA_CXX_FLAGS <flags>
#    EXTRA_DEFINITIONS <flags>
#      This list parameters will append tbe appropriate items.
#
#    NO_UNDEFINED
#      Unresolved symbols will cause an error when making a shared
#      library.
#
#    SANITIZE_[THREADS|ADDRESSES|LEAKS|UNDEFINED]
#      Activate options for the use of the X sanitizer. Notes:
#
#      * The thread sanitizer is mutually exclusive with both the
#      address sanitizer and the leak sanitizer, while the undefined
#      behavior sanitizer may be used with either.
#
#      * Santize behavior may be controlled at the CMake command line
#      with -DCETB_SANITIZE_X=ON|OFF, which overrides these options.
#
#      * This covers only the most basic control of the sanitizer
#      options; if finer control is required, insert the appropriate
#      -fXX options in CXXFLAGS.
#
#      * If CetTest.cmake is used for tests, then ASAN_OPTIONS,
#      TSAN_OPTIONS, LSAN_OPTIONS and UBSAN_OPTIONS will be propagated
#      to the test environment if found in the environment when CMake is
#      executed. Alternatively see CetTest.cmake for per-test options.
#
#      * Due to (a) shortcomings in Root, and (b) an apparent deficiency
#      in some versions of the address sanitizer, it is advised that
#      ASAN_OPTIONS be set to include the options,
#      "detect_leaks=0:new_delete_type_mismatch=0"
#
#      * When executing some applications (such as interactive Root or
#      Gallery) it may be necessary to set the LD_PRELOAD environment
#      variable to include the relevant sanitizer library.
#
#    WERROR
#      All warnings are flagged as errors.
#
####################################
# cet_enable_asserts()
#
#   Enable use of assserts (ie remove -DNDEBUG) regardless of
#   optimization level.
#
####################################
# cet_disable_asserts()
#
#   Disable use of assserts (ie ensure -DNDEBUG) regardless of
#   optimization level.
#
####################################
# cet_maybe_disable_asserts()
#
#   Possibly disable use of assserts (ie ensure -DNDEBUG) based on
#   optimization level.
#
####################################
# cet_add_compiler_flags(<options> <flags>...)
#
#   Add the specified compiler flags.
#
# Options:
#
#   C
#     Add <flags> to CMAKE_C_FLAGS.
#
#   CXX
#    Add <flags> to CMAKE_CXX_FLAGS.
#
#   LANGUAGES <X>
#    Add <flags> to CMAKE_<X>_FLAGS.
#
# Using any or all options is permissible. Using none is equivalent to
# using C CXX.
#
# Duplicates are not removed.
#
####################################
# cet_remove_compiler_flags(<options> <flags>...)
#
#   Remove the specified compiler flags.
#
# Options:
#
#   C
#     Remove <flags> from CMAKE_C_FLAGS.
#
#   CXX <flags>
#     Remove <flags> from CMAKE_CXX_FLAGS.
#
#  LANGUAGES <X>
#     Remove <flags> from CMAKE_<X>_FLAGS.
#
# Using any or all options is permissible. Using none is equivalent to
# using C CXX.
#
####################################
# cet_report_compiler_flags()
#
#   Print the compiler flags currently in use.
#
####################################
# cet_query_system()
#
#   List the values of various variables
#
########################################################################
include(CMakeParseArguments)
include(CetRegexEscape)
include(CheckCXXCompilerFlag)

function( cet_report_compiler_flags )
  string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )
  message( STATUS "compiler flags for directory " ${CURRENT_SUBDIR} " and below")
  message( STATUS "   C++     FLAGS: ${CMAKE_CXX_FLAGS_${BTYPE_UC}}")
  message( STATUS "   C       FLAGS: ${CMAKE_C_FLAGS_${BTYPE_UC}}")
  if (CMAKE_Fortran_COMPILER)
    message( STATUS "   Fortran FLAGS: ${CMAKE_Fortran_FLAGS_${BTYPE_UC}}")
  endif()
endfunction( cet_report_compiler_flags )

macro( _cet_process_flags PTYPE_UC )
   # turn a space separated string into a colon separated list
  STRING( REGEX REPLACE " " ";" tmp_cxx_flags "${CMAKE_CXX_FLAGS_${PTYPE_UC}}")
  STRING( REGEX REPLACE " " ";" tmp_c_flags "${CMAKE_C_FLAGS_${PTYPE_UC}}")
  ##message( STATUS "tmp_cxx_flags: ${tmp_cxx_flags}")
  ##message( STATUS "tmp_c_flags: ${tmp_c_flags}")
  foreach( flag ${tmp_cxx_flags} )
     if( ${flag} MATCHES "^-W(.*)" )
        ##message( STATUS "Warning: ${flag}" )
     elseif( ${flag} MATCHES "-pedantic" )
        ##message( STATUS "Ignoring: ${flag}" )
     elseif( ${flag} MATCHES "-std[=]c[+][+]98" )
        ##message( STATUS "Ignoring: ${flag}" )
     else()
        ##message( STATUS "keep ${flag}" )
        list(APPEND TMP_CXX_FLAGS_${PTYPE_UC} ${flag} )
     endif()
  endforeach( flag )
  foreach( flag ${tmp_c_flags} )
     if( ${flag} MATCHES "^-W(.*)" )
        ##message( STATUS "Warning: ${flag}" )
     elseif( ${flag} MATCHES "-pedantic" )
        ##message( STATUS "Ignoring: ${flag}" )
     else()
        ##message( STATUS "keep ${flag}" )
        list(APPEND TMP_C_FLAGS_${PTYPE_UC} ${flag} )
     endif()
  endforeach( flag )
  ##message( STATUS "TMP_CXX_FLAGS_${PTYPE_UC}: ${TMP_CXX_FLAGS_${PTYPE_UC}}")
  ##message( STATUS "TMP_C_FLAGS_${PTYPE_UC}: ${TMP_C_FLAGS_${PTYPE_UC}}")

endmacro( _cet_process_flags )

macro( cet_base_flags )
  foreach( mytype DEBUG;OPT;PROF )
     ##message( STATUS "checking ${mytype}" )
     _cet_process_flags( ${mytype} )
     ##message( STATUS "${mytype} C   flags: ${TMP_C_FLAGS_${mytype}}")
     ##message( STATUS "${mytype} CXX flags: ${TMP_CXX_FLAGS_${mytype}}")
     set( CET_BASE_CXX_FLAG_${mytype} ${TMP_CXX_FLAGS_${mytype}}
          CACHE STRING "base CXX ${mytype} flags for ups table"
	  FORCE)
     set( CET_BASE_C_FLAG_${mytype} ${TMP_C_FLAGS_${mytype}}
          CACHE STRING "base C ${mytype} flags for ups table"
	  FORCE)
  endforeach( mytype )
  ##message( STATUS "CET_BASE_CXX_FLAG_DEBUG: ${CET_BASE_CXX_FLAG_DEBUG}")
  ##message( STATUS "CET_BASE_CXX_FLAG_OPT:   ${CET_BASE_CXX_FLAG_OPT}")
  ##message( STATUS "CET_BASE_CXX_FLAG_PROF:  ${CET_BASE_CXX_FLAG_PROF}")
endmacro( cet_base_flags )

function(_verify_cxx_std_flag FLAGS FLAG_VAR)
  _find_std_flag(FLAGS FOUND_STD_FLAG)
  _detect_std_flag(CONF_STD_FLAG)

  if (FOUND_STD_FLAG AND CONF_STD_FLAG AND NOT FOUND_STD_FLAG STREQUAL CONF_STD_FLAG)
    message(FATAL_ERROR "Configuration specifies ${CONF_STD_FLAG}, but user CMakeLists.txt specifies ${FOUND_STD_FLAG}.\nPlease change configuration or (preferably) remove user setting of ${FOUND_STD_FLAG}")
  endif()
  if(CONF_STD_FLAG)
    set(${FLAG_VAR} ${CONF_STD_FLAG} PARENT_SCOPE)
  elseif(FOUND_STD_FLAG)
    set(${FLAG_VAR} ${FOUND_STD_FLAG} PARENT_SCOPE)
  endif()
endfunction()

macro( cet_enable_asserts )
  remove_definitions(-DNDEBUG)
endmacro( cet_enable_asserts )

macro( cet_disable_asserts )
  remove_definitions(-DNDEBUG)
  add_definitions(-DNDEBUG)
endmacro( cet_disable_asserts )

macro( cet_maybe_disable_asserts )
  string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )
  cet_enable_asserts() # Starting point
  if( ${BTYPE_UC} MATCHES "OPT" OR
      ${BTYPE_UC} MATCHES "PROF" OR
      ${BTYPE_UC} MATCHES "RELEASE" OR
      ${BTYPE_UC} MATCHES "MINSIZEREL" )
    cet_disable_asserts()
  endif()
endmacro( cet_maybe_disable_asserts )

macro (_parse_flags_options)
  cmake_parse_arguments(CSCF "C;CXX" "" "LANGUAGES" ${ARGN})
  if (CSCF_C)
    list(APPEND CSCF_LANGUAGES "C")
  endif()
  if (CSCF_CXX)
    list(APPEND CSCF_LANGUAGES "CXX")
  endif()
  if (NOT CSCF_LANGUAGES)
    SET(CSCF_LANGUAGES C CXX)
  endif()
endmacro()

macro( cet_add_compiler_flags )
  _parse_flags_options(${ARGN})
  string(REGEX REPLACE ";" " " CSCF_ARGS "${CSCF_UNPARSED_ARGUMENTS}")
  string(REGEX MATCH "(^| )-std=" CSCF_HAVE_STD ${CSCF_ARGS})
  string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )
  # temporary hack while we wait for the real fix
  #_verify_cxx_std_flag(${CSCF_CXX})
  _verify_cxx_std_flag(CSCF_CXX CONF_STD_FLAG)
  foreach(acf_lang ${CSCF_LANGUAGES})
    if (CSCF_HAVE_STD)
      cet_remove_compiler_flags(LANGUAGES ${acf_lang} REGEX "-std=[^ ]*")
    endif()
    set(CMAKE_${acf_lang}_FLAGS_${BTYPE_UC} "${CMAKE_${acf_lang}_FLAGS_${BTYPE_UC}} ${CSCF_ARGS}")
  endforeach()
endmacro( cet_add_compiler_flags )

function(_rm_flag_trim_whitespace VAR FLAG)
  if (NOT ("X${FLAG}" STREQUAL "X"))
    string(REGEX REPLACE "(^| )${FLAG}( |$)" " " ${VAR} "${${VAR}}" )
  endif()
  string(REGEX REPLACE "^ +" "" ${VAR} "${${VAR}}")
  string(REGEX REPLACE " +$" "" ${VAR} "${${VAR}}")
  string(REGEX REPLACE " +" " " ${VAR} "${${VAR}}")
  # Push (local) value of ${${VAR}} up to parent scope.
  set(${VAR} "${${VAR}}" PARENT_SCOPE)
endfunction()

macro( cet_remove_compiler_flags )
  _parse_flags_options(${ARGN})
  cmake_parse_arguments(CSCF "REGEX" "" "" ${CSCF_UNPARSED_ARGUMENTS})
  string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )
  foreach (arg ${CSCF_UNPARSED_ARGUMENTS})
    if (NOT CSCF_REGEX)
      cet_regex_escape("${arg}" arg)
    endif()
    foreach (rcf_lang ${CSCF_LANGUAGES})
      _rm_flag_trim_whitespace(CMAKE_${rcf_lang}_FLAGS_${BTYPE_UC} ${arg})
    endforeach()
  endforeach()
endmacro()

# Find the first -std flag in the incoming list and put it in the
# outgoing var.
function(_find_std_flag IN_VAR OUT_VAR)
  string(REGEX MATCH "(^| )-std=[^ ]*" found_std_flag "${${IN_VAR}}")
  set(${OUT_VAR} "${found_std_flag}" PARENT_SCOPE)
endfunction()

function(_find_extra_std_flags IN_VAR OUT_VAR)
  string(REGEX MATCHALL "(^| )-std=[^ ]*" found_std_flags "${${IN_VAR}}")
  list(LENGTH found_std_flags fsf_len)
  if (fsf_len GREATER 1)
    list(GET found_std_flags 0 tmp)
    set(${OUT_VAR} "${tmp}" PARENT_SCOPE)
  else()
    unset(${OUT_VAR} PARENT_SCOPE)
  endif()
endfunction()

function(_detect_std_flag OUT_VAR)
  check_cxx_compiler_flag("-std=c++17" CPP17_SUPPORTED)
  check_cxx_compiler_flag("-std=c++1z" CPP1Z_SUPPORTED)
  check_cxx_compiler_flag("-std=c++14" CPP14_SUPPORTED)
  check_cxx_compiler_flag("-std=c++1y" CPP1Y_SUPPORTED)
  check_cxx_compiler_flag("-std=c++11" CPP11_SUPPORTED)
  check_cxx_compiler_flag("-std=c++0x" CPP0X_SUPPORTED)
  if(JERM_CXX11 OR JERM_CXX14 OR JERM_CXX17)
    if(JERM_CXX11 AND NOT JERM_CXX14 AND NOT JERM_CXX17)
      if(CPP11_SUPPORTED)
	set(${OUT_VAR} "-std=c++11" PARENT_SCOPE)
      elseif(CPP0X_SUPPORTED)
	set(${OUT_VAR} "-std=c++0x" PARENT_SCOPE)
      else()
	message(FATAL_ERROR "No support for requested C++11 language standard.")
      endif()
    elseif(NOT JERM_CXX11 AND JERM_CXX14 AND NOT JERM_CXX17)
      if(CPP14_SUPPORTED)
	set(${OUT_VAR} "-std=c++14" PARENT_SCOPE)
      elseif(CPP1Y_SUPPORTED)
	set(${OUT_VAR} "-std=c++1y" PARENT_SCOPE)
      else()
	message(FATAL_ERROR "No support for requested C++14 language standard.")
      endif()
    elseif(NOT JERM_CXX11 AND NOT JERM_CXX14 AND JERM_CXX17)
      if(CPP17_SUPPORTED)
	set(${OUT_VAR} "-std=c++17" PARENT_SCOPE)
      elseif(CPP1Z_SUPPORTED)
	set(${OUT_VAR} "-std=c++1z" PARENT_SCOPE)
      else()
	message(FATAL_ERROR "No support for requested C++17 language standard.")
      endif()
    else()
      message(FATAL_ERROR "Conflicting combination of JERM_CXX11, JERM_CXX14 and JERM_CXX17.")
    endif()
  else()
    # No language standard explicitly requested -> find the highest supported by the
    # current compiler. Require at least C++0x
    if(CPP17_SUPPORTED)
      set(${OUT_VAR} "-std=c++17" PARENT_SCOPE)
    elseif(CPP1Z_SUPPORTED)
      set(${OUT_VAR} "-std=c++1z" PARENT_SCOPE)
    elseif(CPP14_SUPPORTED)
      set(${OUT_VAR} "-std=c++14" PARENT_SCOPE)
    elseif(CPP1Y_SUPPORTED)
      set(${OUT_VAR} "-std=c++1y" PARENT_SCOPE)
    elseif(CPP11_SUPPORTED)
      set(${OUT_VAR} "-std=c++11" PARENT_SCOPE)
    elseif(CPP0X_SUPPORTED)
      set(${OUT_VAR} "-std=c++0x" PARENT_SCOPE)
    else()
      message(FATAL_ERROR "Compiler must support at least -std=c++0x. Please upgrade to\nor choose a compiler with proper support for C++11 or better.")
    endif()
  endif()
endfunction()

macro(_remove_extra_std_flags VAR)
  # Remove all but the last -std=nnnn definition from VAR
  string(REGEX MATCHALL "(^| )-std=[^ ]*" found_std_flags "${${VAR}}")
  list(LENGTH found_std_flags fsf_len)
  if (fsf_len GREATER 1)
    list(REMOVE_AT found_std_flags -1)  # Removes last item
    foreach (flag ${found_std_flags})
      cet_regex_escape("${flag}" flag)
      _rm_flag_trim_whitespace(${VAR} "${flag}")
    endforeach()
  endif()
endmacro()

macro(cet_set_compiler_flags)
  cmake_parse_arguments(CSCF
    "ALLOW_DEPRECATIONS;DWARF_STRICT;ENABLE_ASSERTS;NO_UNDEFINED;SANITIZE_ADDRESSES;SANITIZE_LEAKS;SANITIZE_THREADS;SANITIZE_UNDEFINED;WERROR"
    ""
    "DIAGS;DWARF_VER;EXTRA_FLAGS;EXTRA_C_FLAGS;EXTRA_CXX_FLAGS;EXTRA_DEFINITIONS"
    ${ARGN}
    )

  if (CSCF_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unexpected extra arguments: ${CSCF_DEFAULT_ARGS}.\nConsider EXTRA_FLAGS, EXTRA_C_FLAGS, EXTRA_CXX_FLAGS or EXTRA_DEFINITIONS")
  endif()

  _verify_cxx_std_flag(CSCF_EXTRA_CXX_FLAGS CONF_STD_FLAG)

  # turn a semicolon separated list into a space separated string
  STRING( REGEX REPLACE ";" " " CSCF_EXTRA_CXX_FLAGS "${CSCF_EXTRA_CXX_FLAGS}")
  STRING( REGEX REPLACE ";" " " CSCF_EXTRA_C_FLAGS "${CSCF_EXTRA_C_FLAGS}")
  STRING( REGEX REPLACE ";" " " CSCF_EXTRA_FLAGS "${CSCF_EXTRA_FLAGS}")

  ####################################
  # Cavalier
  ####################################
  # C/C++
  set( DFLAGS_CAVALIER "" )
  # C++-only
  set( DXXFLAGS_CAVALIER "" )
  ####################################
  # Cautious
  ####################################
  # C/C++
  set( DFLAGS_CAUTIOUS "${DFLAGS_CAVALIER} -Wall -Werror=return-type" )
  # C++-only
  set( DXXFLAGS_CAUTIOUS "${DXXFLAGS_CAVALIER}" )
  ####################################
  # Vigilant
  ####################################
  # C/C++
  set( DFLAGS_VIGILANT "${DFLAGS_CAUTIOUS} -Wextra -Wno-long-long -Winit-self" )
  if (NOT CMAKE_C_COMPILER MATCHES "/?icc$") # Not understood by ICC
    set( DFLAGS_VIGILANT "${DFLAGS_VIGILANT} -Wno-unused-local-typedefs" )
  endif()
  # C++-only
  set( DXXFLAGS_VIGILANT "${DXXFLAGS_CAUTIOUS} -Woverloaded-virtual -Wnon-virtual-dtor -Wdelete-non-virtual-dtor" )
  ####################################
  # Paranoid
  ####################################
  # C/C++
  set( DFLAGS_PARANOID "${DFLAGS_VIGILANT} -pedantic -Wformat-y2k -Wswitch-default -Wsync-nand -Wtrampolines -Wlogical-op -Wshadow -Wcast-qual" )
  # C++-only
  set( DXXFLAGS_PARANOID "${DXXFLAGS_VIGILANT}" )

  if (NOT CSCF_DIAGS)
    SET(CSCF_DIAGS "CAUTIOUS")
  endif()

  if (CSCF_NO_UNDEFINED)
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,error")
      set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-undefined,error")
    else()
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
      set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined")
    endif()
  elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Make OS X match default SLF6 behavior.
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,dynamic_lookup")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-undefined,dynamic_lookup")
  endif()

  if (CSCF_SANITIZE_THREADS AND CSCF_SANITIZE_LEAKS)
    message(FATAL_ERROR "SANITIZE_THREADS and SANITIZE_LEAKS are mutually exclusive.")
  elseif(CSCF_SANITIZE_THREADS AND CSCF_SANITIZE_ADDRESSES)
    message(FATAL_ERROR "SANITIZE_THREADS and SANITIZE_ADDRESSES are mutually exclusive.")
  endif()

  set(CETB_SANITIZE_ADDRESSES ${CSCF_SANITIZE_ADDRESSES} CACHE BOOL "Select compiler address sanitization." FORCE)
  set(CETB_SANITIZE_LEAKS ${CSCF_SANITIZE_LEAKS} CACHE BOOL "Select compiler leak sanitization." FORCE)
  set(CETB_SANITIZE_THREADS ${CSCF_SANITIZE_THREADS} CACHE BOOL "Select compiler thread sanitization." FORCE)
  set(CETB_SANITIZE_UNDEFINED ${CSCF_SANITIZE_UNDEFINED} CACHE BOOL "Select compiler undefined behavior sanitization." FORCE)

  if (CETB_SANITIZE_THREADS AND CETB_SANITIZE_LEAKS)
    message(FATAL_ERROR "After reconciliation between cet_set_compiler_flags() and CMake option settings, SANITIZE_THREADS and SANITIZE_LEAKS are mutually exclusive.")
  elseif(CETB_SANITIZE_THREADS AND CETB_SANITIZE_ADDRESSES)
    message(FATAL_ERROR "After reconciliation between cet_set_compiler_flags() and CMake option settings, SANITIZE_THREADS and SANITIZE_ADDRESSES are mutually exclusive.")
  endif()

  if (CETB_SANITIZE_THREADS)
    set(SANITIZE_OPTIONS "${SANITIZE_OPTIONS} -fsanitize=thread")
#    if (CMAKE_C_COMPILER_ID STREQUAL GNU)
#      set(SANITIZE_OPTIONS "${SANITIZE_OPTIONS} -static-libtsan")
#    endif()
  endif()

  if (CETB_SANITIZE_ADDRESSES)
    set(SANITIZE_OPTIONS "${SANITIZE_OPTIONS} -fsanitize=address")
#    if (CMAKE_C_COMPILER_ID STREQUAL GNU)
#      set(SANITIZE_OPTIONS "${SANITIZE_OPTIONS} -static-libasan")
#    endif()
  endif()

  if (CETB_SANITIZE_LEAKS)
    set(SANITIZE_OPTIONS "${SANITIZE_OPTIONS} -fsanitize=leak")
#    if (CMAKE_C_COMPILER_ID STREQUAL GNU)
#      set(SANITIZE_OPTIONS "${SANITIZE_OPTIONS} -static-liblsan")
#    endif()
  endif()

  if (CETB_SANITIZE_UNDEFINED)
    set(SANITIZE_OPTIONS "${SANITIZE_OPTIONS} -fsanitize=undefined")
#    if (CMAKE_C_COMPILER_ID STREQUAL GNU)
#      set(SANITIZE_OPTIONS "${SANITIZE_OPTIONS} -static-libubsan")
#    endif()
  endif()

  _set_sanitizer_preloads(TMP_PRELOADS)
  if (TMP_PRELOADS)
    set(CETB_SANITIZER_PRELOADS "${TMP_PRELOADS}" CACHE STRING "Sanitizer pre-load libraries." FORCE)
  endif()

  if (CSCF_WERROR)
    set(CSCF_WERROR "-Werror")
    if (CSCF_ALLOW_DEPRECATIONS)
      set(CSCF_WERROR "${CSCF_WERROR} -Wno-error=deprecated-declarations")
    endif()
  else()
    set(CSCF_WERROR "")
    if (CSCF_ALLOW_DEPRECATIONS)
      message(WARNING "ALLOW_DEPRECATIONS ignored when WERROR not specified")
    endif()
  endif()

  string(TOUPPER "${CSCF_DIAGS}" CSCF_DIAGS)
  if (CSCF_DIAGS STREQUAL "CAVALIER" OR
      CSCF_DIAGS STREQUAL "CAUTIOUS" OR
      CSCF_DIAGS STREQUAL "VIGILANT" OR
      CSCF_DIAGS STREQUAL "PARANOID")
    message(STATUS "Selected diagnostics option ${CSCF_DIAGS}")
  else()
    message(FATAL_ERROR "Unrecognized DIAGS option ${CSCF_DIAGS}")
  endif()

  if (NOT CSCF_DWARF_VER)
    # Default to DWARF4 until our debuggers come up to speed with 5.
    set(CSCF_DWARF_VER 4)
  endif()

  if (CSCF_DWARF_VER EQUAL 2)
    set(GDWARF "-gdwarf-2")
  elseif (CSCF_DWARF_VER EQUAL 3)
    set(GDWARF "-gdwarf-3")
  elseif (CSCF_DWARF_VER EQUAL 4)
    set(GDWARF "-gdwarf-4")
  elseif (CSCF_DWARF_VER EQUAL 5)
    set(GDWARF "-gdwarf-5")
  elseif (CSCF_DWARF_VER)
    message(FATAL_ERROR "Unexpected value of DWARF_VER: ${CSCF_DWARF_VER}. Valid values are 2 - 5.")
  endif()

  if (GDWARF AND CSCF_DWARF_STRICT)
    set(GDWARF "${GDWARF} -gstrict-dwarf")
  endif()

  set(C_FLAGS_OMNIBUS "${CSCF_WERROR} ${CSCF_EXTRA_FLAGS} ${CSCF_EXTRA_C_FLAGS} ${DFLAGS_${CSCF_DIAGS}} ${SANITIZE_OPTIONS}")
  set(CXX_FLAGS_OMNIBUS "-std=c++98 ${CSCF_WERROR} ${CSCF_EXTRA_FLAGS} ${CONF_STD_FLAG} ${CSCF_EXTRA_CXX_FLAGS} ${DFLAGS_${CSCF_DIAGS}} ${DXXFLAGS_${CSCF_DIAGS}} ${SANITIZE_OPTIONS}")

  # DEBUG
  set( CMAKE_C_FLAGS_DEBUG "-g ${GDWARF} -O0 ${C_FLAGS_OMNIBUS}" )
  set( CMAKE_CXX_FLAGS_DEBUG "-g ${GDWARF} -O0 ${CXX_FLAGS_OMNIBUS}"  )
  # MINSIZEREL
  set( CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${C_FLAGS_OMNIBUS}" )
  set( CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${CXX_FLAGS_OMNIBUS}" )
  # RELEASE
  set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${C_FLAGS_OMNIBUS}" )
  set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS_OMNIBUS}" )
  # RELWITHDEBINFO
  set( CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${C_FLAGS_OMNIBUS}" )
  set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CXX_FLAGS_OMNIBUS}" )
  # OPT
  # set( CMAKE_C_FLAGS_OPT "-g ${GDWARF} -O3 ${C_FLAGS_OMNIBUS}" CACHE STRING "Flags used by the C compiler for optimized builds." FORCE)
  # set( CMAKE_CXX_FLAGS_OPT "-g ${GDWARF} -O3 ${CXX_FLAGS_OMNIBUS}" CACHE STRING "Flags used by the C++ compiler for optimized builds." FORCE)
  # # PROF
  # set( CMAKE_C_FLAGS_PROF "-g ${GDWARF} -O3 -fno-omit-frame-pointer ${C_FLAGS_OMNIBUS}" CACHE STRING "Flags used by the C compiler for profile builds." FORCE)
  # set( CMAKE_CXX_FLAGS_PROF "-g ${GDWARF} -O3 -fno-omit-frame-pointer ${CXX_FLAGS_OMNIBUS}" CACHE STRING "Flags used by the C++ compiler for profile builds." FORCE)
  # MARK_AS_ADVANCED(
  #   CMAKE_CXX_FLAGS_OPT
  #   CMAKE_C_FLAGS_OPT
  #   CMAKE_EXE_LINKER_FLAGS_OPT
  #   CMAKE_STATIC_LINKER_FLAGS_OPT
  #   CMAKE_SHARED_LINKER_FLAGS_OPT
  #   CMAKE_CXX_FLAGS_PROF
  #   CMAKE_C_FLAGS_PROF
  #   CMAKE_EXE_LINKER_FLAGS_PROF
  #   CMAKE_SHARED_LINKER_FLAGS_PROF
  #   CMAKE_STATIC_LINKER_FLAGS_PROF
  #   )

  # Linker Flags
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SANITIZE_OPTIONS}" )
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULELINKER_FLAGS} ${SANITIZE_OPTIONS}" )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZE_OPTIONS}" )

  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
  endif()

  # Update the documentation string of CMAKE_BUILD_TYPE for GUIs
  SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE )

  # Leave this in definitions.
  cet_remove_compiler_flags(-DNDEBUG)

  string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )
  remove_definitions(-DNDEBUG)
  if ( CSCF_ENABLE_ASSERTS )
    cet_enable_asserts()
  else()
    cet_maybe_disable_asserts()
  endif()
  add_definitions(${CSCF_EXTRA_DEFINITIONS})
  
  #message( STATUS "compiling with ${CMAKE_BASE_NAME} ${CMAKE_CXX_FLAGS}")

  get_directory_property( CSCF_CD COMPILE_DEFINITIONS )
  if( CSCF_CD )
    message( STATUS "   DEFINE (-D): ${CSCF_CD}")
  endif()
 
  _remove_extra_std_flags(CMAKE_C_FLAGS_${BTYPE_UC})
  _remove_extra_std_flags(CMAKE_CXX_FLAGS_${BTYPE_UC})
  _remove_extra_std_flags(CMAKE_Fortran_FLAGS_${BTYPE_UC})

  _find_std_flag(CMAKE_CXX_FLAGS found_flag)
  if (found_flag)
    check_cxx_compiler_flag(found_flag compiler_has_std)
    if (NOT compiler_has_std)
      message( FATAL_ERROR "CXX compiler does not support ${found_flag}")
    endif()
  endif()

endmacro( cet_set_compiler_flags )

function(_report_var VAR)
  message(STATUS "${VAR} is ${${VAR}}")
endfunction()

# This macro is useful if you need to check a variable
# http://cmake.org/Wiki/CMake_Useful_Variables#Compilers_and_Tools also
# see
# http://cmake.org/Wiki/CMake_Useful_Variables/Logging_Useful_Variables
function( cet_query_system )
  message( STATUS "cet_query_system: begin compiler report")
  _report_var(CMAKE_SYSTEM_NAME)
  _report_var(CMAKE_BASE_NAME)
  _report_var(CMAKE_BUILD_TYPE)
  _report_var(CMAKE_CONFIGURATION_TYPES)
  _report_var(BUILD_SHARED_LIBS)
  _report_var(CMAKE_C_COMPILER_ID)
  _report_var(CMAKE_CXX_COMPILER_ID)
  _report_var(CMAKE_Fortran_COMPILER_ID)
  _report_var(CMAKE_COMPILER_IS_GNUCXX)
  _report_var(CMAKE_COMPILER_IS_MINGW)
  _report_var(CMAKE_COMPILER_IS_CYGWIN)
  _report_var(CMAKE_AR)
  _report_var(CMAKE_RANLIB)
  _report_var(CMAKE_CXX_COMPILER)
  _report_var(CMAKE_CXX_OUTPUT_EXTENSION)
  _report_var(CMAKE_CXX_FLAGS_DEBUG)
  _report_var(CMAKE_CXX_FLAGS_RELEASE)
  _report_var(CMAKE_CXX_FLAGS_MINSIZEREL)
  _report_var(CMAKE_CXX_FLAGS_RELWITHDEBINFO)
  # _report_var(CMAKE_CXX_FLAGS_OPT)
  # _report_var(CMAKE_CXX_FLAGS_PROF)
  _report_var(CMAKE_CXX_STANDARD_LIBRARIES)
  _report_var(CMAKE_CXX_LINK_FLAGS)
  _report_var(CMAKE_C_COMPILER)
  _report_var(CMAKE_C_FLAGS)
  _report_var(CMAKE_C_FLAGS_DEBUG)
  _report_var(CMAKE_C_FLAGS_RELEASE)
  _report_var(CMAKE_C_FLAGS_MINSIZEREL)
  _report_var(CMAKE_C_FLAGS_RELWITHDEBINFO)
  # _report_var(CMAKE_C_FLAGS_OPT)
  # _report_var(CMAKE_C_FLAGS_PROF)
  _report_var(CMAKE_C_OUTPUT_EXTENSION)
  _report_var(CMAKE_SHARED_LIBRARY_CXX_FLAGS)
  _report_var(CMAKE_SHARED_MODULE_CXX_FLAGS)
  _report_var(CMAKE_STATIC_LIBRARY_CXX_FLAGS)
  _report_var(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
  _report_var(CMAKE_SHARED_LINKER_FLAGS)
  _report_var(CMAKE_SHARED_LINKER_FLAGS_DEBUG)
  _report_var(CMAKE_SHARED_LINKER_FLAGS_RELEASE)
  _report_var(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL)
  _report_var(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO)
  # _report_var(CMAKE_SHARED_LINKER_FLAGS_OPT)
  # _report_var(CMAKE_SHARED_LINKER_FLAGS_PROF)
  _report_var(CMAKE_STATIC_LINKER_FLAGS)
  _report_var(CMAKE_STATIC_LINKER_FLAGS_DEBUG)
  _report_var(CMAKE_STATIC_LINKER_FLAGS_RELEASE)
  _report_var(CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL)
  _report_var(CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO)
  # _report_var(CMAKE_STATIC_LINKER_FLAGS_OPT)
  # _report_var(CMAKE_STATIC_LINKER_FLAGS_PROF)
  _report_var(CMAKE_MODULE_LINKER_FLAGS)
  _report_var(CMAKE_MODULE_LINKER_FLAGS_DEBUG)
  _report_var(CMAKE_MODULE_LINKER_FLAGS_RELEASE)
  _report_var(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL)
  _report_var(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO)
  # _report_var(CMAKE_MODULE_LINKER_FLAGS_OPT)
  # _report_var(CMAKE_MODULE_LINKER_FLAGS_PROF)
  _report_var(CMAKE_EXE_LINKER_FLAGS)
  _report_var(CMAKE_EXE_LINKER_FLAGS_DEBUG)
  _report_var(CMAKE_EXE_LINKER_FLAGS_RELEASE)
  _report_var(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL)
  _report_var(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO)
  # _report_var(CMAKE_EXE_LINKER_FLAGS_OPT)
  # _report_var(CMAKE_EXE_LINKER_FLAGS_PROF)
  message( STATUS "cet_query_system: end compiler report")
endfunction( cet_query_system )

function(_set_sanitizer_preloads VAR)
  if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    set(hints HINTS $ENV{GCC_FQ_DIR}/lib64 NO_DEFAULT_PATH)
  elseif(CMAKE_C_COMPILER_ID STREQUAL Clang)
    set(hints HINTS $ENV{CLANG_FQ_DIR} NO_DEFAULT_PATH)
  endif()
  if (CETB_SANITIZE_ADDRESSES)
    find_library (ASAN_PRELOAD NAMES libasan.so
      ${hints}
      DOC "Found location for the address sanitizer preload library."
      )
    if (ASAN_PRELOAD)
      string(APPEND TMP_PRELOADS "${ASAN_PRELOAD}")
    endif()
  endif()
  if (CETB_SANITIZE_LEAKS)
    find_library (LSAN_PRELOAD NAMES liblsan.so
      ${hints}
      DOC "Found location for the leak sanitizer preload library."
      )
    if (LSAN_PRELOAD)
      string(APPEND TMP_PRELOADS " ${LSAN_PRELOAD}")
    endif()
  endif()
  if (CETB_SANITIZE_THREADS)
    find_library (TSAN_PRELOAD NAMES libtsan.so
      ${hints}
      DOC "Found location for the thread sanitizer preload library."
      )
    if (TSAN_PRELOAD)
      string(APPEND TMP_PRELOADS " ${TSAN_PRELOAD}")
    endif()
  endif()
  if (CETB_SANITIZE_UNDEFINED)
    find_library (UBSAN_PRELOAD NAMES libubsan.so
      ${hints}
      DOC "Found location for the undefined behavior sanitizer preload library."
      )
    if (UBSAN_PRELOAD)
      string(APPEND TMP_PRELOADS " ${UBSAN_PRELOAD}")
    endif()
  endif()
  set(${VAR} ${TMP_PRELOADS} PARENT_SCOPE)
endfunction()
