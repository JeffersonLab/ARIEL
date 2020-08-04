# Throw a fatal error if cmake is invoked from within the source code directory tree
# cet_ensure_out_of_source_build()
#

execute_process(COMMAND /bin/pwd -P
  WORKING_DIRECTORY /
  OUTPUT_QUIET
  ERROR_QUIET
  RESULT_VARIABLE _cet_pwd_status
  )

if (_cet_pwd_status EQUAL 0)
  set (_cet_pwd_P_arg "-P" CACHE INTERNAL "/bin/pwd args")
else()
  set (_cet_pwd_P_arg "" CACHE INTERNAL "/bin/pwd args")
endif()

function (_cet_real_dir INPUT_DIR OUTPUT_VAR)
  execute_process(COMMAND /bin/pwd ${_cet_pwd_P_arg}
    WORKING_DIRECTORY "${INPUT_DIR}"
    OUTPUT_VARIABLE result
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  set (${OUTPUT_VAR} "${result}" PARENT_SCOPE)
endfunction()

macro (cet_ensure_out_of_source_build)

##message(STATUS "cet_ensure_out_of_source_build: CMAKE_SOURCE_DIR is ${CMAKE_SOURCE_DIR}")
##message(STATUS "cet_ensure_out_of_source_build: CMAKE_BINARY_DIR is ${CMAKE_BINARY_DIR}")

  _cet_real_dir("${CMAKE_SOURCE_DIR}" cet_source_real)
  _cet_real_dir("${CMAKE_BINARY_DIR}" cet_build_real)
  string(COMPARE EQUAL "${cet_source_real}" "${cet_build_real}" in_source)
  string( REGEX MATCH "${cet_source_real}/" maybe_in_source_subdir "${cet_build_real}")

  ##message(STATUS "cet_ensure_out_of_source_build: ${in_source} and ${maybe_in_source_subdir}")
  if( maybe_in_source_subdir )
    _cet_real_dir("${CMAKE_CURRENT_BINARY_DIR}" thisdir)
    ##message(STATUS "/bin/pwd returns ${thisdir}")
    string( REGEX MATCH "${cet_source_real}/" in_source_subdir "${thisdir}")
    ##message(STATUS "regex match returns --${in_source_subdir}--")
  endif ()
  
  ##message(STATUS "cet_ensure_out_of_source_build: in_source_subdir is ${in_source_subdir}")
  if (in_source OR in_source_subdir)
  message(FATAL_ERROR "
ERROR: In source builds of this project are not allowed.
A separate build directory is required.
Please create one and run cmake from the build directory.
Also note that cmake has just added files to your source code directory.
We suggest getting a new copy of the source code.
Otherwise, delete `CMakeCache.txt' and the directory `CMakeFiles'.
  ")
  endif ()

endmacro (cet_ensure_out_of_source_build)

