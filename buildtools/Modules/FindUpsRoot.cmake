# Find ROOT in environment and define useful variables (library locations etc.)
#
# find_ups_root(  [minimum] )
#  minimum - optional minimum version

include(CheckUpsVersion)
include(CMakeParseArguments)

function(_set_root_lib_vars)
  file(GLOB root_libs
    LIST_DIRECTORIES false
    ${ROOT_LIB_DIR}/lib*.so ${ROOT_LIB_DIR}/lib*.a)
  foreach (root_lib_path ${root_libs})
    if (NOT root_lib_path MATCHES "Dict\\.")
      get_filename_component(root_lib_stem ${root_lib_path} NAME_WE)
      string(REGEX REPLACE "^lib" "" ROOTLIB ${root_lib_stem})
      string(TOUPPER ${ROOTLIB} ROOTLIB_UC)
      set(ROOT_${ROOTLIB_UC} ${root_lib_path} PARENT_SCOPE)
    endif()
  endforeach()
endfunction()

function(_set_and_check_prog VAR PROG)
  if (NOT EXISTS ${PROG})
    message(SEND_ERROR "find_ups_root: expected ROOT program ${PROG} missing.")
  endif()
  set(${VAR} ${PROG} PARENT_SCOPE)
endfunction()

# since variables are passed, this is implemented as a macro
macro( find_ups_root )

  # require either ROOTSYS or root-config
  set( ROOTSYS $ENV{ROOTSYS} )
  find_program(ROOT_CONFIG_EXEC root-config PATHS ${ROOTSYS}/bin DOC "ROOT configuration utility program")
  if(ROOT_CONFIG_EXEC)
    execute_process(
      COMMAND ${ROOT_CONFIG_EXEC} --prefix
      OUTPUT_VARIABLE ROOTSYS
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    #message( STATUS "ROOTSYS = ${ROOTSYS}" )
  else()
    message(FATAL_ERROR "Cannot find ROOT")
  endif()

  # only execute if this macro has not already been called
  if( NOT ROOT_VERSION )
    ##message( STATUS "find_ups_root debug: ROOT_VERSION is NOT defined" )

    execute_process(COMMAND ${ROOT_CONFIG_EXEC} --version
      OUTPUT_VARIABLE ROOT_CONFIG_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    message( STATUS "ROOT version: ${ROOT_CONFIG_VERSION}" )
    string( REGEX REPLACE "^([0-9]+)\\.([0-9]+)/([0-9]+).*" "v\\1_\\2_\\3" ROOT_VERSION "${ROOT_CONFIG_VERSION}" )

    cmake_parse_arguments( FUR "" "" "" ${ARGN} )
    set( minimum )
    if( FUR_UNPARSED_ARGUMENTS )
      list( GET FUR_UNPARSED_ARGUMENTS 0 minimum )
      #message( STATUS "find_ups_root: checking root ${ROOT_VERSION} against ${minimum}" )
      _check_version( ROOT ${ROOT_VERSION} ${minimum} )
    endif()

    _get_dotver( ${ROOT_VERSION} )
    set( ROOT_DOT_VERSION ${dotver} )
    # compare for recursion
    list(FIND cet_product_list root found_product_match)
    if( ${found_product_match} LESS 0 )
      # add to product list
      list(APPEND CONFIG_FIND_UPS_COMMAND_LIST "find_ups_root( ${minimum} )")
      set(cet_product_list root ${cet_product_list} )
    endif()

    execute_process(
      COMMAND ${ROOT_CONFIG_EXEC} --bindir
      OUTPUT_VARIABLE ROOT_BIN_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT ROOT_BIN_DIR)
      message( FATAL_ERROR "No ROOT bindir")
    endif()
    execute_process(
      COMMAND ${ROOT_CONFIG_EXEC} --libdir
      OUTPUT_VARIABLE ROOT_LIB_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT ROOT_LIB_DIR)
      message( FATAL_ERROR "No ROOT libdir")
    endif()
    execute_process(
      COMMAND ${ROOT_CONFIG_EXEC} --incdir
      OUTPUT_VARIABLE ROOT_INC_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    if(NOT ROOT_INC_DIR)
      message( FATAL_ERROR "No ROOT incdir")
    endif()

    include_directories ( ${ROOT_INC_DIR} )
    set(CMAKE_LIBRARY_PATH "${ROOT_LIB_DIR}" "${CMAKE_LIBRARY_PATH}")
    # if(APPLE)
    #   set(CMAKE_BUILD_RPATH "${ROOT_LIB_DIR}" "${CMAKE_BUILD_RPATH}")
    # endif()

    check_ups_version(root ${ROOT_VERSION} v6_00_00
      PRODUCT_OLDER_VAR HAVE_ROOT5
      PRODUCT_MATCHES_VAR HAVE_ROOT6
      )

    check_ups_version(root ${ROOT_VERSION} v6_10_04
      PRODUCT_MATCHES_VAR ROOT6_HAS_NOINCLUDEPATHS
      )

    check_ups_version(root ${ROOT_VERSION} v6_10_04
      PRODUCT_MATCHES_VAR HAVE_ROOT_NAMESPACE_TARGETS
      )

    # Set library variables
    _set_root_lib_vars()

    # define genreflex executable
    _set_and_check_prog(ROOT_GENREFLEX ${ROOT_BIN_DIR}/genreflex)

    # check for the need to cleanup after genreflex
    check_ups_version(root ${ROOT_VERSION} v5_28_00d PRODUCT_OLDER_VAR GENREFLEX_CLEANUP)
    #message(STATUS "genreflex cleanup status: ${GENREFLEX_CLEANUP}")

    # define rootcint executable
    if (HAVE_ROOT6)
      _set_and_check_prog(ROOTCLING ${ROOT_BIN_DIR}/rootcling)
    else() # ROOT5
      _set_and_check_prog(ROOTCINT ${ROOT_BIN_DIR}/rootcint)
    endif()

    if (HAVE_ROOT_NAMESPACE_TARGETS)
      # This command exposes all allowed targets to downstream users
      find_package(ROOT REQUIRED)
    endif()

    if(NOT ROOT_PYTHON_VERSION)
      execute_process(
        COMMAND ${ROOT_CONFIG_EXEC} --python-version
        OUTPUT_VARIABLE ROOT_PYTHON_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()

    # define some useful library lists
    set(ROOT_BASIC_LIB_LIST ${ROOT_CORE}
      ${ROOT_CINT}
      ${ROOT_RIO}
      ${ROOT_NET}
      ${ROOT_IMT}
      ${ROOT_HIST}
      ${ROOT_GRAF}
      ${ROOT_GRAF3D}
      ${ROOT_GPAD}
      ${ROOT_TREE}
      ${ROOT_RINT}
      ${ROOT_POSTSCRIPT}
      ${ROOT_MATRIX}
      ${ROOT_PHYSICS}
      ${ROOT_MATHCORE}
      ${ROOT_THREAD}
      )
    set(ROOT_GUI_LIB_LIST   ${ROOT_GUI} ${ROOT_BASIC_LIB_LIST} )
    set(ROOT_EVE_LIB_LIST   ${ROOT_EVE}
      ${ROOT_EG}
      ${ROOT_TREEPLAYER}
      ${ROOT_GEOM}
      ${ROOT_GED}
      ${ROOT_RGL}
      ${ROOT_GUI_LIB_LIST}
      )
  endif( NOT ROOT_VERSION )

endmacro( find_ups_root )
