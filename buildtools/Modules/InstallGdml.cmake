########################################################################
#
# install_gdml()
#   Install gdml scripts in a top level gdml subdirectory
#   Default extensions:
#     .gdml
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
# install_gdml( [SUBDIRS subdirectory_list]
#                [EXTRAS extra_files]
#                [EXCLUDES exclusions] )
# install_gdml( LIST file_list )
#
########################################################################

include(CMakeParseArguments)
include(CetCurrentSubdir)
include (CetCopy)
include (CetExclude)

macro( _cet_copy_gdml )
  cmake_parse_arguments( CPGDML "" "SUBDIR;WORKING_DIRECTORY" "LIST" ${ARGN})
  get_filename_component( gdmlpathname ${gdml_install_dir} NAME )
  set( gdmlbuildpath ${PROJECT_BINARY_DIR}/${gdmlpathname} )
  if( CPGDML_SUBDIR )
    set( gdmlbuildpath "${gdmlbuildpath}/${CPGDML_SUBDIR}" )
  endif( CPGDML_SUBDIR )
  if (CPGDML_WORKING_DIRECTORY)
    cet_copy(${CPGDML_LIST} DESTINATION "${gdmlbuildpath}" WORKING_DIRECTORY "${CPGDML_WORKING_DIRECTORY}")
  else()
    cet_copy(${CPGDML_LIST} DESTINATION "${gdmlbuildpath}")
  endif()
  #message(STATUS "_cet_copy_gdml: copying to ${gdmlbuildpath}")
endmacro( _cet_copy_gdml )

macro( _cet_install_gdml_without_list   )
  #message( STATUS "gdml scripts will be installed in ${gdml_install_dir}" )
  FILE(GLOB gdml_files [^.]*.gdml [^.]*.C [^.]*.xml [^.]*.xsd README )
  if( IGDML_EXCLUDES )
    _cet_exclude_from_list( gdml_files EXCLUDES ${IGDML_EXCLUDES} LIST ${gdml_files} )
  endif()
  if( gdml_files )
    #message( STATUS "installing gdml files ${gdml_files} in ${gdml_install_dir}")
    _cet_copy_gdml( LIST ${gdml_files} )
    INSTALL ( FILES ${gdml_files}
              DESTINATION ${gdml_install_dir} )
  endif( gdml_files )
  # now check subdirectories
  if( IGDML_SUBDIRS )
    foreach( sub ${IGDML_SUBDIRS} )
      FILE(GLOB subdir_gdml_files
                ${sub}/[^.]*.gdml  
		${sub}/[^.]*.C 
		${sub}/[^.]*.xml 
		${sub}/[^.]*.xsd 
		${sub}/README
		)
      #message( STATUS "found ${sub} files ${subdir_gdml_files}")
      if( IGDML_EXCLUDES )
        _cet_exclude_from_list( subdir_gdml_files EXCLUDES ${IGDML_EXCLUDES} LIST ${subdir_gdml_files} )
      endif()
      if( subdir_gdml_files )
        _cet_copy_gdml( LIST ${subdir_gdml_files} SUBDIR ${sub} )
        INSTALL ( FILES ${subdir_gdml_files}
                  DESTINATION ${gdml_install_dir}/${sub} )
      endif( subdir_gdml_files )
    endforeach(sub)
  endif( IGDML_SUBDIRS )
endmacro( _cet_install_gdml_without_list )

macro( install_gdml   )
  cmake_parse_arguments( IGDML "" "" "SUBDIRS;LIST;EXTRAS;EXCLUDES" ${ARGN})
  set(gdml_install_dir ${${product}_gdml_dir} )
  _cet_debug_message( "install_gdml: gdml scripts will be installed in ${gdml_install_dir}" )
  #_cet_debug_message( "install_gdml: IGDML_SUBDIRS is ${IGDML_SUBDIRS}")

  if( IGDML_LIST )
    if( IGDML_SUBDIRS )
      message( FATAL_ERROR
               "ERROR: call install_gdml with EITHER LIST or SUBDIRS but not both")
    endif( IGDML_SUBDIRS )
    _cet_copy_gdml( LIST ${IGDML_LIST} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    INSTALL ( FILES  ${IGDML_LIST}
              DESTINATION ${gdml_install_dir} )
  else()
    if( IGDML_EXTRAS )
      _cet_copy_gdml( LIST ${IGDML_EXTRAS} )
      INSTALL ( FILES  ${IGDML_EXTRAS}
                DESTINATION ${gdml_install_dir} )
    endif( IGDML_EXTRAS )
    _cet_install_gdml_without_list()
  endif()
endmacro( install_gdml )
