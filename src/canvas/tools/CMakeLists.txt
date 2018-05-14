
# use a list 
# these are all scripts
# add a LIST option if there are files which are not scripts
macro( _canvas_install_tools )
  cmake_parse_arguments( CT "" "DIRNAME" "SCRIPT_LIST" ${ARGN})
  if( CT_DIRNAME )
    install( PROGRAMS ${CT_SCRIPT_LIST}
             DESTINATION "${${product}_data_dir}/tools/${CT_DIRNAME}" )
  else()
    install( PROGRAMS ${CT_SCRIPT_LIST}
             DESTINATION "${${product}_data_dir}/tools" )
  endif()
endmacro( _canvas_install_tools )

_canvas_install_tools( SCRIPT_LIST find-param-req make-branch )

add_subdirectory (canvas_refactor)
add_subdirectory (migration)
