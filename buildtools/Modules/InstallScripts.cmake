########################################################################
#
# install_scripts()
#   Install scripts in the package binary directory.
#   Default extensions:
#     .sh .py .pl .rb [.cfg when AS_TEST is specified]
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
# The AS_TEST option for install_scripts will install scripts in a test subdirectory.
#
####################################
# Recommended use:
#
# install_scripts( [SUBDIRS subdirectory_list]
#                  [EXTRAS extra_files]
#                  [EXCLUDES exclusions]
#                  [AS_TEST] )
# install_scripts( LIST file_list [AS_TEST] )
#
########################################################################

include(CMakeParseArguments)
include(CetCurrentSubdir)
include (CetCopy)
include (CetExclude)

macro( _cet_install_script_without_list   )
  _cet_debug_message( "_cet_install_script_without_list: scripts will be installed in ${script_install_dir}" )
  if( IS_AS_TEST )
    FILE(GLOB scripts [^.]*.sh [^.]*.py [^.]*.pl [^.]*.rb [^.]*.cfg )
  else()
    FILE(GLOB scripts [^.]*.sh [^.]*.py [^.]*.pl [^.]*.rb )
  endif()
  if( IS_EXCLUDES )
    _cet_exclude_from_list( scripts EXCLUDES ${IS_EXCLUDES} LIST ${scripts} )
  endif()
  if( scripts )
    #message( STATUS "installing scripts ${scripts} in ${script_install_dir}")
    INSTALL ( PROGRAMS ${scripts}
              DESTINATION ${script_install_dir} )
  endif( scripts )
  # now check subdirectories
  if( IS_SUBDIRS )
    foreach( sub ${IS_SUBDIRS} )
      if( IS_AS_TEST )
	FILE(GLOB subdir_scripts
                  ${sub}/[^.]*.sh ${sub}/[^.]*.py ${sub}/[^.]*.pl ${sub}/[^.]*.rb ${sub}/[^.]*.cfg )
      else()
	FILE(GLOB subdir_scripts
                  ${sub}/[^.]*.sh ${sub}/[^.]*.py ${sub}/[^.]*.pl ${sub}/[^.]*.rb )
      endif()
      if( IS_EXCLUDES )
        _cet_exclude_from_list( subdir_scripts EXCLUDES ${IS_EXCLUDES} LIST ${subdir_scripts} )
      endif()
      if( subdir_scripts )
        INSTALL ( PROGRAMS ${subdir_scripts}
                  DESTINATION ${script_install_dir} )
      endif( subdir_scripts )
    endforeach(sub)
  endif( IS_SUBDIRS )
endmacro( _cet_install_script_without_list )

macro( install_scripts   )
  cmake_parse_arguments( IS "AS_TEST" "" "SUBDIRS;LIST;EXTRAS;EXCLUDES" ${ARGN})
  if( IS_AS_TEST )
    set(script_install_dir ${${product}_test_dir} )
  else()
    set(script_install_dir ${${product}_bin_dir} )
  endif()
  _cet_debug_message( "install_scripts: scripts will be installed in ${script_install_dir}" )
  if( IS_LIST )
    if( IS_SUBDIRS )
      message( FATAL_ERROR
               "ERROR: call install_scripts with EITHER LIST or SUBDIRS but not both")
    endif( IS_SUBDIRS )
    INSTALL ( PROGRAMS  ${IS_LIST}
              DESTINATION ${script_install_dir} )
  else()
    if( IS_EXTRAS )
      INSTALL ( PROGRAMS  ${IS_EXTRAS}
                DESTINATION ${script_install_dir} )
    endif( IS_EXTRAS )
    _cet_install_script_without_list()
  endif()
endmacro( install_scripts )
