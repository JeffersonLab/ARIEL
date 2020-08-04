########################################################################
# install_fw([SUBDIRNAME dir] LIST files...)
#   Install FW data in ${${CMAKE_PROJECT_NAME}_fw_dir}/${SUBDIRNAME}
#
####################################
# Recommended use:
#
# install_fw( LIST file_list 
#             [SUBDIRNAME subdirectory_under_fwdir] )
# THERE ARE NO DEFAULTS FOR install_fw
#
########################################################################

include(CMakeParseArguments)
include(CetCurrentSubdir)
include (CetCopy)
include (CetExclude)

macro( _cet_copy_fw )
  cmake_parse_arguments( CPFW "" "SUBDIRNAME;WORKING_DIRECTORY" "LIST" ${ARGN})
  set( mrb_build_dir $ENV{MRB_BUILDDIR} )
  get_filename_component( fwpathname ${fw_install_dir} NAME )
  #message(STATUS "_cet_copy_fw: copying to mrb ${mrb_build_dir}/${product}/${fwpathname} or cet ${CETPKG_BUILD}/${fwpathname}")
  if( mrb_build_dir )
    set( fwbuildpath ${mrb_build_dir}/${product}/${fwpathname} )
  else()
    set( fwbuildpath ${CETPKG_BUILD}/${fwpathname} )
  endif()
  if( CPFW_SUBDIRNAME )
    set( fwbuildpath ${fwbuildpath}/${CPFW_SUBDIRNAME} )
  endif( CPFW_SUBDIRNAME )
  if (CPFW_WORKING_DIRECTORY)
    cet_copy(${CPFW_LIST} DESTINATION "${fwbuildpath}" WORKING_DIRECTORY "${CPFW_WORKING_DIRECTORY}")
  else()
    cet_copy(${CPFW_LIST} DESTINATION "${fwbuildpath}")
  endif()
  #message(STATUS "_cet_copy_fw: copying to ${fwbuildpath}")
endmacro( _cet_copy_fw )

macro( install_fw   )
  cmake_parse_arguments( IFW "" "SUBDIRNAME" "LIST" ${ARGN})
  set( fw_install_dir ${${product}_fw_dir} )
  _cet_debug_message( "install_fw: fw scripts will be installed in ${fw_install_dir}" )

  if( IFW_LIST )
    _cet_copy_fw( ${ARGN} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" )
    if ( IFW_SUBDIRNAME )
      INSTALL ( FILES  ${IFW_LIST}
                DESTINATION ${fw_install_dir}/${IFW_SUBDIRNAME} )
    else()
      INSTALL ( FILES  ${IFW_LIST}
                DESTINATION ${fw_install_dir} )
    endif()
  else()
      message( FATAL_ERROR "ERROR: install_fw has no defaults, you must use LIST")
  endif()
endmacro( install_fw )
