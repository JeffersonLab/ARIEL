# internal function
# not currently in use, but retained for future refernce

function (_cet_prepend_directory OUTPUT_VAR prefix)
  set( fileList "" )
  #message(STATUS "prepend ${prefix} to ${ARGN}")
  foreach( myfile ${ARGN} )
      get_filename_component( mydir "${myfile}" DIRECTORY )
      if( IS_ABSOLUTE "${mydir}" )
           list(APPEND fileList "${myfile}")
      else()
          list(APPEND fileList "${prefix}/${myfile}")
      endif()
  endforeach()
  #message(STATUS "_cet_prepend_directory new list ${fileList}" )
  set (${OUTPUT_VAR} "${fileList}" PARENT_SCOPE)
  #message(STATUS "_cet_prepend_directory output ${OUTPUT_VAR}" )
endfunction()
