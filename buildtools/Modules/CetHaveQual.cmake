########################################################################
# cet_have_qual
#
# Search for a particular qualifier string (e.g. "a7" in "a7:debug")
#
# Usage:
#
# cet_have_qual( <qualifier> [REGEX] [<ans-var>])
#
# <ans-var> (or CET_HAVE_QUAL if not specified) will be set in the
# caller's scope to TRUE if <qualifier> may be found in the current set,
# FALSE if not.
#
########################################################################
include(CMakeParseArguments)

function( cet_have_qual findq )
  cmake_parse_arguments(CHQ "REGEX" "" "" ${ARGN})
  list(LENGTH CHQ_UNPARSED_ARGUMENTS chq_def_args_length)
  if (chq_def_args_length GREATER 0)
    list(GET CHQ_UNPARSED_ARGUMENTS 0 ans_var)
  else()
    set(ans_var CET_HAVE_QUAL)
  endif()
  if (CHQ_REGEX)
    set(qual_index -1)
    STRING(REGEX MATCH "(^|:)${findq}(:|$)" found_match "${ARIEL_BUILD_OPTS}")
    if (found_match)
      set(qual_index 0)
    endif()
  else()
    STRING( REGEX REPLACE ":" ";" qualifier_as_list "${ARIEL_BUILD_OPTS}" )
    list(FIND qualifier_as_list ${findq} qual_index)
    #message(STATUS "cet_have_qual: qual_index is ${qual_index}")
  endif()
  if( qual_index LESS 0 )
    set( ${ans_var} "FALSE" PARENT_SCOPE) # Not found.
  else()
    set( ${ans_var} "TRUE" PARENT_SCOPE) # Found.
  endif()
  #message(STATUS "cet_have_qual: returning ${CET_HAVE_QUAL}")
endfunction(cet_have_qual)

