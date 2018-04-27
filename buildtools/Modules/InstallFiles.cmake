########################################################################
# cet_install_files()
#   Install files
#   
#  General utility to install read-only files.  Not suitable for scripts.
#
####################################
# Recommended use:
#
# cet_install_files( LIST file_list 
#                DIRNAME directory name
#                [FQ_DIR]
#               )
#
#  If FQ_DIR is not specified, files will be installed in PRODUCT_DIR/<directory name>
#  If FQ_DIR is specified, files will be installed in PRODUCT_FQ_DIR/<directory name>
# 

include(CMakeParseArguments)
include(CetCurrentSubdir)
include (CetCopy)

function( cet_install_files )
  cmake_parse_arguments( IFG "FQ_DIR" "DIRNAME" "LIST" ${ARGN})
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
    set(this_install_dir "${product}/${version}/${IFG_DIRNAME}" )
  endif()
  _cet_debug_message( "cet_install_files: files will be installed in ${this_install_dir}" )

  # copy to build directory
  set( mrb_build_dir $ENV{MRB_BUILDDIR} )
  if( mrb_build_dir )
    set( this_build_path ${mrb_build_dir}/${product}/${IFG_DIRNAME} )
  else()
    set( this_build_path ${CETPKG_BUILD}/${IFG_DIRNAME} )
  endif()

  cet_copy( ${IFG_LIST} DESTINATION "${this_build_path}" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" )
  INSTALL ( FILES  ${IFG_LIST}
            DESTINATION "${this_install_dir}" )

endfunction( cet_install_files )

