########################################################################
# art_dictionary
#
# Wrapper around cetbuildtools' build_dictionary featuring the addition
# of commonly required libraries to the dictionary library link list.
#
# Note that use of check_class_version has been delegated to
# build_dictionary(). The only difference between build_dictionary() and
# art_dictionary() is now the aforementioned addition of commonly
# required libraries to the dictionary library link list.
#
####################################
# Options and Arguments
#
# COMPILE_FLAGS
#   Passed through to build_dictionary.
#
# DICT_FUNCTIONS
#   Passed through to build_dictionary.
#
# DICT_NAME_VAR
#   Passed through to build_dictionary.
#
# DICTIONARY_LIBRARIES
#   Passed through to build_dictionary with additions.
#
# REQUIRED_DICTIONARIES
#   Passed through to build_dictionaries.
#
# NO_CHECK_CLASS_VERSION
#   Passed through to build_dictionaries.
#
# NO_DEFAULT_LIBRARIES
#   Do not add the usual set of default libraries to the
#   DICTIONARY_LIBRARIES list: equivalent to calling build_dictionary
#   directly.
#
# UPDATE_IN_PLACE
#   Passed through to build_dictionary.
#
# USE_PRODUCT_NAME
#   Passed through to build_dictionary.
#
# RECURSIVE
#   Passed through to build_dictionary.
#
# NO_RECURSIVE
#   Passed through to build_dictionary.
#
#########################################################################
include(BuildDictionary)
include(CMakeParseArguments)

function(art_dictionary)
  check_ups_version(cetbuildtools ${cetbuildtools_UPS_VERSION} v5_0_0
    PRODUCT_MATCHES_VAR AD_HAVE_REQUIRED_CETBUILDTOOLS)
  if (NOT AD_HAVE_REQUIRED_CETBUILDTOOLS)
    message(FATAL_ERROR "ArtDictionary requires cetbuildtools v5_0_0 or greater.")
  endif()

  cmake_parse_arguments(AD
    "UPDATE_IN_PLACE;DICT_FUNCTIONS;USE_PRODUCT_NAME;NO_CHECK_CLASS_VERSION;NO_DEFAULT_LIBRARIES"
    "DICT_NAME_VAR"
    "DICTIONARY_LIBRARIES;COMPILE_FLAGS;REQUIRED_DICTIONARIES"
    ${ARGN}
    )
  if (NOT AD_NO_DEFAULT_LIBRARIES)
    set(AD_DICTIONARY_LIBRARIES
      canvas cetlib cetlib_except ${AD_DICTIONARY_LIBRARIES}
      )
  endif()
  set(extra_args "")
  if (AD_DICT_FUNCTIONS)
    list(APPEND extra_args DICT_FUNCTIONS)
  endif()
  if (AD_USE_PRODUCT_NAME)
    list(APPEND extra_args USE_PRODUCT_NAME)
  endif()
  if (AD_NO_CHECK_CLASS_VERSION)
    list(APPEND extra_args NO_CHECK_CLASS_VERSION)
  endif()
  if (AD_UPDATE_IN_PLACE)
    list(APPEND extra_args UPDATE_IN_PLACE)
  endif()
  if (AD_COMPILE_FLAGS)
    list(APPEND extra_args COMPILE_FLAGS ${AD_COMPILE_FLAGS})
  endif()
  build_dictionary(${AD_UNPARSED_ARGUMENTS}
    ${extra_args}
    DICT_NAME_VAR dictname
    DICTIONARY_LIBRARIES ${AD_DICTIONARY_LIBRARIES}
    )
  if (cet_generated_code) # Bubble up to top scope.
    set(cet_generated_code ${cet_generated_code} PARENT_SCOPE)
  endif()
  if (AD_DICT_NAME_VAR)
    set (${AD_DICT_NAME_VAR} ${dictname} PARENT_SCOPE)
  endif()
endfunction()
