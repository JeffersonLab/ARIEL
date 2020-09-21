########################################################################
# install_source()
#   Install source code for debugging purposes.
#   Default extensions:
#      .cc .c .cpp .C .cxx
#      .h .hh .H .hpp .icc .tcc
#      .xml
#     .sh .py .pl .rb
#
# The SUBDIRS option allows you to search subdirectories (e.g. a detail subdirectory)
#
# The EXTRAS option is intended to allow you to pick up extra files not otherwise found.
# They should be specified by relative path (eg f1, subdir1/f2, etc.).
#
# The EXCLUDES option will exclude the specified files from the installation list.
#
# The LIST option allows you to install from a list. When LIST is used,
# we do not search for other files to install. Note that the LIST and
# SUBDIRS options are mutually exclusive.
#
####################################
# Recommended use:
#
# install_source( [SUBDIRS subdirectory_list] 
#                 [EXTRAS extra_files] 
#                 [EXCLUDES exclusions] )
# install_source( LIST file_list )
#
########################################################################

include(CMakeParseArguments)
include(CetCurrentSubdir)
include (CetCopy)
include (CetExclude)

macro( _cet_check_build_directory )
  FILE(GLOB build_directory_files
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.cc
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.c
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.cpp
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.C
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.cxx
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.h
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.hh
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.H
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.hpp
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.icc
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.tcc
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.xml
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.sh
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.py
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.pl
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.rb
	    )
  FILE(GLOB build_directory_headers
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.h
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.hh
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.H
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.hpp
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.icc
	    ${CMAKE_CURRENT_BINARY_DIR}/[^.]*.tcc
	    )
  if( build_directory_files )
    #message( STATUS "_cet_check_build_directory: installing ${build_directory_files} in ${source_install_dir}")
    INSTALL( FILES ${build_directory_files}
             DESTINATION ${source_install_dir} )
  endif( build_directory_files )
  if( build_directory_headers )
    #message( STATUS "_cet_check_build_directory: installing ${build_directory_headers} in ${source_install_dir}")
    INSTALL( FILES ${build_directory_headers}
             DESTINATION ${header_install_dir} )
  endif( build_directory_headers )
endmacro( _cet_check_build_directory )

macro( _cet_install_generated_dictionary_code )
  # check for dictionary code
  if( cet_generated_code )
    foreach( dict ${cet_generated_code} )
      INSTALL( FILES ${dict}
        DESTINATION ${source_install_dir} )
    endforeach(dict)
  endif( cet_generated_code )
  set(cet_generated_code) # Clear to avoid causing problems in subdirectories.
endmacro( _cet_install_generated_dictionary_code )

macro( _cet_install_without_list   )
  #message( STATUS "source code will be installed in ${source_install_dir}" )
  FILE(GLOB src_files
	    [^.]*.cc [^.]*.c [^.]*.cpp [^.]*.C [^.]*.cxx
	    [^.]*.h [^.]*.hh [^.]*.H [^.]*.hpp [^.]*.icc [^.]*.tcc
	    [^.]*.xml [^.]*.sh [^.]*.py [^.]*.pl [^.]*.rb
	    *README* [^.]*.md [^.]*.dox
	    )
  #message( STATUS "debug: src_files ${src_files}" )
  if( ISRC_EXCLUDES )
    _cet_exclude_from_list( src_files EXCLUDES ${ISRC_EXCLUDES} LIST ${src_files} )
  endif()
  if( src_files )
    foreach( f ${src_files} )
      if( NOT f MATCHES ".bak" AND NOT f MATCHES "~" AND NOT IS_DIRECTORY ${f} )
	#message( STATUS "debug: installing ${f}" )
	INSTALL( FILES ${f}
        	 DESTINATION ${source_install_dir} )
       endif()
     endforeach()
  endif( src_files )
  # check for generated files
  _cet_check_build_directory()
  _cet_install_generated_dictionary_code()
  # now check subdirectories
  if( ISRC_SUBDIRS )
     foreach( sub ${ISRC_SUBDIRS} )
	FILE(GLOB subdir_src_files
        	 ${sub}/[^.]*.cc ${sub}/[^.]*.c ${sub}/[^.]*.cpp ${sub}/[^.]*.C ${sub}/[^.]*.cxx
        	 ${sub}/[^.]*.h ${sub}/[^.]*.hh ${sub}/[^.]*.H ${sub}/[^.]*.hpp ${sub}/[^.]*.icc ${sub}/[^.]*.tcc
        	 ${sub}/[^.]*.xml ${sub}/[^.]*.sh ${sub}/[^.]*.py ${sub}/[^.]*.pl ${sub}/[^.]*.rb
		 ${sub}/*README* ${sub}/[^.]*.md ${sub}/[^.]*.dox )
	if( ISRC_EXCLUDES )
          _cet_exclude_from_list( subdir_src_files EXCLUDES ${ISRC_EXCLUDES} LIST ${subdir_src_files} )
	endif()
	if( subdir_src_files )
	  foreach( f ${subdir_src_files} )
	    if( NOT f MATCHES ".bak" AND NOT f MATCHES "~" AND NOT IS_DIRECTORY ${f} )
	      #message( STATUS "debug: installing ${f}" )
	      INSTALL( FILES ${f}
        	       DESTINATION ${source_install_dir}/${sub} )
	     endif()
	   endforeach()
	endif( subdir_src_files )
     endforeach(sub)
     #message( STATUS "also installing in subdirectories: ${ISRC_SUBDIRS}")
  endif( ISRC_SUBDIRS )
endmacro( _cet_install_without_list )

macro( _cet_install_from_list  source_files  )
  #message( STATUS "_cet_install_from_list debug: source code list will be installed in ${source_install_dir}" )
  #message( STATUS "_cet_install_from_list debug: install list is ${source_files}")
  INSTALL( FILES ${source_files}
           DESTINATION ${source_install_dir} )
endmacro( _cet_install_from_list )

macro( install_source   )
  cmake_parse_arguments( ISRC "" "" "SUBDIRS;LIST;EXTRAS;EXCLUDES" ${ARGN})
  #message( STATUS "install_source: PACKAGE_TOP_DIRECTORY is ${PACKAGE_TOP_DIRECTORY}")
  _cet_current_subdir( CURRENT_SUBDIR )
  if( ARIEL_INSTALL_SRCDIR )
    set(source_install_dir ${ARIEL_INSTALL_SRCDIR}${CURRENT_SUBDIR} )
  else()
    set(source_install_dir src/${product}/${version}${CURRENT_SUBDIR} )
  endif()
  ##message( STATUS "install_source: source code will be installed in ${source_install_dir}" )
  if( ISRC_LIST )
    if( ISRC_SUBDIRS )
      message( FATAL_ERROR
               "ERROR: call install_source with EITHER LIST or SUBDIRS but not both")
    endif( ISRC_SUBDIRS )
    _cet_install_from_list("${ISRC_LIST}")
  else()
    if( ISRC_EXTRAS )
      _cet_install_from_list("${ISRC_EXTRAS}")
    endif( ISRC_EXTRAS )
    _cet_install_without_list()
  endif()
endmacro( install_source )
