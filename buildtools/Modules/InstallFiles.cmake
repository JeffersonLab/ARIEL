########################################################################
# cet_install_files()
#   Install files
#   
#  General utility to install read-only files.  Not suitable for scripts.
#
####################################
# Recommended use:
#
# cet_install_files(LIST file_list
#                   DIRNAME directory name
#                   [INSTALL_ONLY]
#                   [FQ_DIR]
#                  )
#
#  If FQ_DIR is not specified, files will be installed in PRODUCT_DIR/<directory name>
#  If FQ_DIR is specified, files will be installed in PRODUCT_FQ_DIR/<directory name>
#  If INSTALL_ONLY is specified, files will *not* be copied to the build tree.

include(CMakeParseArguments)
include (CetCopy)

function( cet_install_files )
  cmake_parse_arguments( IFG "FQ_DIR;INSTALL_ONLY" "DIRNAME" "LIST" ${ARGN})
  set( cet_install_files_usage "USAGE: cet_install_files( DIRNAME <directory name> LIST <file list> [FQ_DIR] )")

  if ( NOT IFG_DIRNAME )
    message( FATAL_ERROR "DIRNAME is required \n ${cet_install_files_usage}")
  endif( NOT IFG_DIRNAME )
  if ( NOT IFG_LIST )
    message( FATAL_ERROR "LIST is required \n ${cet_install_files_usage}")
  endif( NOT IFG_LIST )

  if ( IFG_FQ_DIR )
    set(this_install_dir "${flavorqual_dir}/${IFG_DIRNAME}" )
  else()
    set(this_install_dir "${IFG_DIRNAME}" )
  endif()
  _cet_debug_message( "cet_install_files: files will be installed in ${this_install_dir}" )

  # copy to build directory
  set( this_build_path ${PROJECT_BINARY_DIR}/${IFG_DIRNAME} )

  if (NOT IFG_INSTALL_ONLY)
    cet_copy( ${IFG_LIST} DESTINATION "${this_build_path}" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" )
  endif()
  INSTALL ( FILES  ${IFG_LIST}
            DESTINATION "${this_install_dir}" )

endfunction( cet_install_files )

