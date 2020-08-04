# cet_cmake_env
#
# factor out the boiler plate at the top of every main CMakeLists.txt file
# cet_cmake_env( [arch] )
# allow optional architecture declaration
# noarch is recognized, others are used at your own discretion
########################################################################

####################################
# This clause is needed *only* whilst clients of cetbuildtools try and
# locate it via the envvar stanza before including this module Once
# migration to standard find_package use is complete, this block should
# be removed
if(NOT cetbuildtools_BINDIR AND DEFINED ENV{CETBUILDTOOLS_DIR})
    set(cetbuildtools_BINDIR "$ENV{CETBUILDTOOLS_DIR}/bin")
endif()
####################################
message(STATUS "cetbuildtools_BINDIR = ${cetbuildtools_BINDIR}")


# Dummy use of CET_TEST_GROUPS to quell warning.
if (CET_TEST_GROUPS)
endif()

include(CetGetProductInfo)
include(CetRegexEscape)
include(CetHaveQual)

function(_cet_real_file FILEPATH VAR)
  get_filename_component(dir "${FILEPATH}" DIRECTORY)
  _cet_real_dir("${dir}" dir)
  get_filename_component(file "${FILEPATH}" NAME)
  set(${VAR} "${dir}/${file}" PARENT_SCOPE)
endfunction()

# Verify that the compiler is set as desired, and is consistent with our
# current known use of qualifiers.

function(_verify_cc COMPILER)
  if(NOT CMAKE_C_COMPILER) # Languages disabled.
    return()
  endif()
  if(COMPILER STREQUAL "cc")
    set(compiler_ref "/usr/bin/cc")
  elseif(COMPILER MATCHES "^(gcc.*)$")
    set(compiler_ref "$ENV{GCC_FQ_DIR}/bin/${CMAKE_MATCH_0}")
  elseif(COMPILER STREQUAL icc)
    set(compiler_ref "$ENV{ICC_FQ_DIR}/bin/intel64/${COMPILER}")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    set(compiler_ref "$ENV{APPLE_CLANG_FQ_DIR}/bin/${COMPILER}")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(compiler_ref "$ENV{CLANG_FQ_DIR}/bin/${COMPILER}")
  elseif(COMPILER MATCHES "[-_]gcc\\$")
    message(FATAL_ERROR "Cross-compiling not yet supported")
  else()
    message(FATAL_ERROR "Unrecognized C compiler \"${COMPILER}\": use cc, gcc(-XXX)?, icc, or clang.")
  endif()
  _cet_real_file("${compiler_ref}" compiler_ref)
  cet_regex_escape("${compiler_ref}" escaped_path)
  _cet_real_file("${CMAKE_C_COMPILER}" compiler_path)
  if(NOT ("${compiler_path}" MATCHES "^${escaped_path}\$"))
    message(FATAL_ERROR "CMAKE_C_COMPILER real path is ${compiler_path}: expected match to \"^${escaped_path}\\\$\".\n"
      "Use buildtool or preface cmake invocation with \"env CC=${CETPKG_CC}.\" Use buildtool -c if changing qualifier.")
  endif()
endfunction()

function(_verify_cxx COMPILER)
  if(NOT CMAKE_CXX_COMPILER) # Languages disabled.
    return()
  endif()
  if(COMPILER STREQUAL "c++")
    set(compiler_ref "/usr/bin/c++")
  elseif(COMPILER MATCHES "^(g\\+\\+.*)$")
    set(compiler_ref "$ENV{GCC_FQ_DIR}/bin/${CMAKE_MATCH_0}")
  elseif(COMPILER STREQUAL icpc)
    set(compiler_ref "$ENV{ICC_FQ_DIR}/bin/intel64/${COMPILER}")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(compiler_ref "$ENV{APPLE_CLANG_FQ_DIR}/bin/${COMPILER}")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(compiler_ref "$ENV{CLANG_FQ_DIR}/bin/${COMPILER}")
  elseif(COMPILER MATCHES "[-_]g\\+\\+$")
    message(FATAL_ERROR "Cross-compiling not yet supported")
  else()
    message(FATAL_ERROR "Unrecognized C++ compiler \"${COMPILER}\": use c++, g++(-XXX)?, icpc, or clang++.")
  endif()
  _cet_real_file("${compiler_ref}" compiler_ref)
  cet_regex_escape("${compiler_ref}" escaped_path)
  _cet_real_file("${CMAKE_CXX_COMPILER}" compiler_path)
  if(NOT ("${compiler_path}" MATCHES "^${escaped_path}\$"))
    message(FATAL_ERROR "CMAKE_CXX_COMPILER real path is ${compiler_path}: expected match to \"^${escaped_path}\\\$\".\n"
      "Use buildtool or preface cmake invocation with \"env CXX=${CETPKG_CXX}.\" Use buildtool -c if changing qualifier.")
  endif()
endfunction()

function(_verify_fc COMPILER)
  if(NOT CMAKE_Fortran_COMPILER) # Languages disabled.
    return()
  endif()
  if(COMPILER MATCHES "^(gfortran.*)$")
    cet_regex_escape("$ENV{GCC_FQ_DIR}/bin/${CMAKE_MATCH_0}" escaped_path)
    set(compiler_ref "^${escaped_path}$")
  elseif(COMPILER STREQUAL ifort)
    set(compiler_ref "$ENV{ICC_FQ_DIR}/bin/intel64/${COMPILER}")
  elseif(COMPILER STREQUAL clang)
    message(FATAL_ERROR "Clang not yet supported.")
  elseif(COMPILER MATCHES "[-_]gfortran$")
    message(FATAL_ERROR "Cross-compiling not yet supported")
  else()
    message(FATAL_ERROR "Unrecognized Fortran compiler \"${COMPILER}\": use , gfortran(-XXX)? or ifort.")
  endif()
  get_filename_component(cr_dir "${compiler_ref}" DIRECTORY)
  _cet_real_dir("${cr_dir}" cr_dir)
  get_filename_component(cr_name "${compiler_ref}" NAME)
  set(compiler_ref "${cr_dir}/${cr_name}")
  if(NOT (CMAKE_Fortran_COMPILER MATCHES "${compiler_ref}"))
    message(FATAL_ERROR "CMAKE_Fortran_COMPILER set to ${CMAKE_Fortran_COMPILER}: expected match to \"${compiler_ref}\".\n"
      "Use buildtool or preface cmake invocation with \"env FC=${CETPKG_FC}.\" Use buildtool -c if changing qualifier.")
  endif()
endfunction()

function(_study_compiler CTYPE)
  # CTYPE = CC, CXX or FC
  if (NOT CTYPE STREQUAL "CC" AND
      NOT CTYPE STREQUAL "CXX" AND
      NOT CTYPE STREQUAL "FC")
    message(FATAL_ERROR "INTERNAL ERROR: unrecognized CTYPE ${CTYPE} to _study_compiler")
  endif()
  cet_get_product_info_item(${CTYPE} rcompiler ec_compiler)
  if (NOT rcompiler)
    message(FATAL_ERROR "Unable to obtain compiler suite setting: re-source setup_for_development?")
  endif()
  if (CTYPE STREQUAL "CC")
    _verify_cc(${rcompiler})
  elseif(CTYPE STREQUAL "CXX")
    _verify_cxx(${rcompiler})
  elseif(CTYPE STREQUAL "FC")
    _verify_fc(${rcompiler})
  else()
    message(FATAL_ERROR "INTERNAL ERROR: case missing for CTYPE ${CTYPE} in _study_compiler")
  endif()
endfunction()

function(_verify_compiler_quals)
  _study_compiler(CC)
  _study_compiler(FC)
  _study_compiler(CXX)
endfunction()

macro(_get_cetpkg_info)

  cet_get_product_info_item(product rproduct ec_product)
  if(ec_product)
    message(FATAL_ERROR "Unable to obtain product information: need to re-source setup_for_development?")
  endif()

  cet_get_product_info_item(version rversion)
  cet_get_product_info_item(default_version rdefault_version)
  cet_get_product_info_item(qualifier rqual)

  set(product ${rproduct} CACHE STRING "Package UPS name" FORCE)
  set(version ${rversion} CACHE STRING "Package UPS version" FORCE)
  set(default_version ${rdefault_version} CACHE STRING "Package UPS default version" FORCE)
  set( mrb_qual $ENV{MRB_QUALS} )
  if ( mrb_qual )
    set(full_qualifier ${mrb_qual} CACHE STRING "Package UPS full_qualifier" FORCE)
  else()
    set(full_qualifier ${rqual} CACHE STRING "Package UPS full_qualifier" FORCE)
  endif()
  set(${product}_full_qualifier ${rqual} CACHE STRING "Package UPS ${product}_full_qualifier" FORCE)
  #message(STATUS "_get_cetpkg_info: found ${product} ${version} ${${product}_full_qualifier}")

  set( cet_ups_dir ${CMAKE_CURRENT_SOURCE_DIR}/ups CACHE STRING "Package UPS directory" FORCE )
  ##message( STATUS "_get_cetpkg_info: cet_ups_dir is ${cet_ups_dir}")
endmacro(_get_cetpkg_info)

macro(cet_cmake_env)

  # project() must have been called before us.
  if(NOT CMAKE_PROJECT_NAME)
    message (FATAL_ERROR
      "CMake project() command must have been invoked prior to cet_cmake_env()."
      "\nIt must be invoked at the top level, not in an included .cmake file.")
  endif()

  option(BUILD_SHARED_LIBS "Build shared libraries for this project." ON)

  _get_cetpkg_info()
  
  # temporarily set this policy
  # silently ignore non-existent dependencies
  # deprecated with predudice as of cmake 3.11.xx
  # new behavior is to report an error if non-existent dependencies are 
  # listed in the ``add_dependencies()`` command.
  cmake_policy(SET CMP0046 NEW)

  # Acknowledge new RPATH behavior on OS X.
  cmake_policy(SET CMP0042 NEW)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
  endif()


  if( ${product}_full_qualifier )
    # extract base qualifier
    STRING( REGEX REPLACE ":debug" "" Q1 "${${product}_full_qualifier}" )
    STRING( REGEX REPLACE ":opt" "" Q2 "${Q1}" )
    STRING( REGEX REPLACE ":prof" "" Q3 "${Q2}" )
    set(qualifier ${Q3} CACHE STRING "Package UPS qualifier" FORCE)
    if(qualifier)
      # NOP to quell warning
    endif()
    message( STATUS "full qual ${${product}_full_qualifier} reduced to ${qualifier}")
  endif()

  message(STATUS "Product is ${product} ${version} ${${product}_full_qualifier}")
  message(STATUS "Module path is ${CMAKE_MODULE_PATH}")

  set_install_root()
  enable_testing()
  
  include(CetParseArgs)
  cet_parse_args( EOSB "" "ALLOW_IN_SOURCE_BUILD" ${ARGN})
  if( EOSB_DEFAULT_ARGS)
    set(arch "${EOSB_DEFAULT_ARGS}")
  endif()
  # Ensure out of source build before anything else
  if( NOT EOSB_ALLOW_IN_SOURCE_BUILD )
    include(EnsureOutOfSourceBuild)
    cet_ensure_out_of_source_build()
  endif()
  ##message(STATUS "cet_cmake_env debug: ${arch} ${EOSB_DEFAULT_ARGS}")

  # Useful includes.
  include(FindUpsPackage)
  include(FindUpsBoost)
  include(FindUpsRoot)
  include(FindUpsGeant4)
  include(ParseUpsVersion)
  include(SetCompilerFlags)
  include(SetFlavorQual)
  include(InstallFhicl)
  include(InstallFiles)
  include(InstallFW)
  include(InstallGdml)
  include(InstallHeaders)
  include(InstallLicense)
  include(InstallPerllib)
  include(InstallScripts)
  include(InstallSource)
  include(InstallWP)
  include(CetCMakeUtils)
  include(CetMake)
  include(CetCMakeConfig)

  # initialize cmake config file fragments
  _cet_init_config_var()

  # Make sure compiler is set as the configuration requires.
  if( "${arch}" MATCHES "noarch" )
  message(STATUS "${product} is null flavored")
  else()
  _verify_compiler_quals()
  endif()

  #set package version from ups version
  set_version_from_ups( ${version} )
  # look for the case where there are no underscores
  string(REGEX MATCHALL "_" nfound ${version} )
  list(LENGTH nfound nfound)
  ##message(STATUS "project version components: ${VERSION_MAJOR} ${VERSION_MINOR} ${VERSION_PATCH} ${VERSION_TWEAK}" )
  if( ${nfound} EQUAL 0 )
     set( cet_dot_version ${VERSION_MAJOR} CACHE STRING "Package dot version" FORCE)
  else()
     set_dot_version( ${product} ${version} )
     ##message(STATUS "project dot version: ${${PRODUCTNAME_UC}_DOT_VERSION}" )
     set( cet_dot_version  ${${PRODUCTNAME_UC}_DOT_VERSION} CACHE STRING "Package dot version" FORCE)
  endif()
  message(STATUS "cet dot version: ${cet_dot_version}" )
  # find $CETBUILDTOOLS_DIR/bin/cet_report
  set( CETBUILDTOOLS_DIR $ENV{CETBUILDTOOLS_DIR} )
  set(CET_REPORT ${cetbuildtools_BINDIR}/cet_report)
  message(STATUS "CET_REPORT: ${CET_REPORT}")
  #define flavorqual and flavorqual_dir
  set_flavor_qual( ${arch} )
  cet_set_lib_directory()
  cet_set_bin_directory()
  cet_set_inc_directory()
  cet_set_fcl_directory()
  cet_set_fw_directory()
  cet_set_gdml_directory()
  cet_set_perllib_directory()
  cet_set_wp_directory()
  cet_set_test_directory()

  if (ENV{CETPKB_BUILD})
    set(CETPKG_BUILD $ENV{CETPKG_BUILD})
  elseif (ENV{MRB_BUILDDIR})
    set(CETPKG_BUILD $ENV{MRB_BUILDDIR})
  else()
    set(CETPKG_BUILD ${PROJECT_BINARY_DIR})
  endif()

  # add to the include path
  include_directories ("${PROJECT_BINARY_DIR}")
  include_directories("${PROJECT_SOURCE_DIR}" )
  # make sure all libraries are in one directory
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
  # make sure all executables are in one directory
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
  # install license and readme if found
  install_license()
  
endmacro(cet_cmake_env)

macro(cet_check_gcc)
  message(WARNING "Obsolete function cet_check_gcc called -- NOP.")
endmacro(cet_check_gcc)

macro( cet_set_lib_directory )
  execute_process(COMMAND ${CET_REPORT} libdir ${cet_ups_dir}
    OUTPUT_VARIABLE REPORT_LIB_DIR_MSG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
  #message( STATUS "${CET_REPORT} libdir returned ${REPORT_LIB_DIR_MSG}")
  if( ${REPORT_LIB_DIR_MSG} MATCHES "DEFAULT" )
     set( ${product}_lib_dir ${flavorqual_dir}/lib CACHE STRING "Package lib directory" FORCE )
  elseif( ${REPORT_LIB_DIR_MSG} MATCHES "NONE" )
     set( ${product}_lib_dir ${REPORT_LIB_DIR_MSG} CACHE STRING "Package lib directory" FORCE )
  elseif( ${REPORT_LIB_DIR_MSG} MATCHES "ERROR" )
     set( ${product}_lib_dir ${REPORT_LIB_DIR_MSG} CACHE STRING "Package lib directory" FORCE )
  else()
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" ldir1 "${REPORT_LIB_DIR_MSG}" )
    STRING( REGEX REPLACE "product_dir" "${product}/${version}" ldir2 "${ldir1}" )
    set( ${product}_lib_dir ${ldir2}  CACHE STRING "Package lib directory" FORCE )
  endif()
  #message( STATUS "cet_set_lib_directory: ${product}_lib_dir is ${${product}_lib_dir}")
endmacro( cet_set_lib_directory )

macro( cet_set_bin_directory )
  execute_process(COMMAND ${CET_REPORT} bindir ${cet_ups_dir}
    OUTPUT_VARIABLE REPORT_BIN_DIR_MSG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
  #message( STATUS "${CET_REPORT} bindir returned ${REPORT_BIN_DIR_MSG}")
  if( ${REPORT_BIN_DIR_MSG} MATCHES "DEFAULT" )
     set( ${product}_bin_dir ${flavorqual_dir}/bin CACHE STRING "Package bin directory" FORCE )
  elseif( ${REPORT_BIN_DIR_MSG} MATCHES "NONE" )
     set( ${product}_bin_dir ${REPORT_BIN_DIR_MSG} CACHE STRING "Package bin directory" FORCE )
  elseif( ${REPORT_BIN_DIR_MSG} MATCHES "ERROR" )
     set( ${product}_bin_dir ${REPORT_BIN_DIR_MSG} CACHE STRING "Package bin directory" FORCE )
  else()
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" bdir1 "${REPORT_BIN_DIR_MSG}" )
    STRING( REGEX REPLACE "product_dir" "${product}/${version}" bdir2 "${bdir1}" )
    set( ${product}_bin_dir ${bdir2}  CACHE STRING "Package bin directory" FORCE )
  endif()
  #message( STATUS "cet_set_bin_directory: ${product}_bin_dir is ${${product}_bin_dir}")
endmacro( cet_set_bin_directory )

macro( cet_set_fcl_directory )
  execute_process(COMMAND ${CET_REPORT} fcldir ${cet_ups_dir}
    OUTPUT_VARIABLE REPORT_FCL_DIR_MSG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
  #message( STATUS "${CET_REPORT} fcldir returned ${REPORT_FCL_DIR_MSG}")
  if( ${REPORT_FCL_DIR_MSG} MATCHES "DEFAULT" )
     set( ${product}_fcl_dir ${product}/${version}/fcl CACHE STRING "Package fcl directory" FORCE )
  elseif( ${REPORT_FCL_DIR_MSG} MATCHES "NONE" )
     set( ${product}_fcl_dir ${REPORT_FCL_DIR_MSG} CACHE STRING "Package fcl directory" FORCE )
  elseif( ${REPORT_FCL_DIR_MSG} MATCHES "ERROR" )
     set( ${product}_fcl_dir ${REPORT_FCL_DIR_MSG} CACHE STRING "Package fcl directory" FORCE )
  else()
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" fdir1 "${REPORT_FCL_DIR_MSG}" )
    STRING( REGEX REPLACE "product_dir" "${product}/${version}" fdir2 "${fdir1}" )
    set( ${product}_fcl_dir ${fdir2}  CACHE STRING "Package fcl directory" FORCE )
  endif()
  #message( STATUS "cet_set_fcl_directory: ${product}_fcl_dir is ${${product}_fcl_dir}")
endmacro( cet_set_fcl_directory )

macro( cet_set_fw_directory )
  execute_process(COMMAND ${CET_REPORT} fwdir ${cet_ups_dir}
    OUTPUT_VARIABLE REPORT_FW_DIR_MSG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
  #message( STATUS "${CET_REPORT} fwdir returned ${REPORT_FW_DIR_MSG}")
   if( ${REPORT_FW_DIR_MSG} MATCHES "DEFAULT" )
     set( ${product}_fw_dir "NONE" CACHE STRING "Package fw directory" FORCE )
  elseif( ${REPORT_FW_DIR_MSG} MATCHES "NONE" )
     set( ${product}_fw_dir ${REPORT_FW_DIR_MSG} CACHE STRING "Package fw directory" FORCE )
  elseif( ${REPORT_FW_DIR_MSG} MATCHES "ERROR" )
     set( ${product}_fw_dir ${REPORT_FW_DIR_MSG} CACHE STRING "Package fw directory" FORCE )
  else()
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" fdir1 "${REPORT_FW_DIR_MSG}" )
    STRING( REGEX REPLACE "product_dir" "${product}/${version}" fdir2 "${fdir1}" )
    set( ${product}_fw_dir ${fdir2}  CACHE STRING "Package fw directory" FORCE )
  endif()
  #message( STATUS "cet_set_fw_directory: ${product}_fw_dir is ${${product}_fw_dir}")
endmacro( cet_set_fw_directory )

macro( cet_set_gdml_directory )
  execute_process(COMMAND ${CET_REPORT} gdmldir ${cet_ups_dir}
    OUTPUT_VARIABLE REPORT_GDML_DIR_MSG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
  #message( STATUS "${CET_REPORT} gdmldir returned ${REPORT_GDML_DIR_MSG}")
  if( ${REPORT_GDML_DIR_MSG} MATCHES "DEFAULT" )
     set( ${product}_gdml_dir "NONE" CACHE STRING "Package gdml directory" FORCE )
  elseif( ${REPORT_GDML_DIR_MSG} MATCHES "NONE" )
     set( ${product}_gdml_dir ${REPORT_GDML_DIR_MSG} CACHE STRING "Package gdml directory" FORCE )
  elseif( ${REPORT_GDML_DIR_MSG} MATCHES "ERROR" )
     set( ${product}_gdml_dir ${REPORT_GDML_DIR_MSG} CACHE STRING "Package gdml directory" FORCE )
  else()
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" fdir1 "${REPORT_GDML_DIR_MSG}" )
    STRING( REGEX REPLACE "product_dir" "${product}/${version}" fdir2 "${fdir1}" )
    set( ${product}_gdml_dir ${fdir2}  CACHE STRING "Package gdml directory" FORCE )
  endif()
  #message( STATUS "cet_set_gdml_directory: ${product}_gdml_dir is ${${product}_gdml_dir}")
endmacro( cet_set_gdml_directory )

macro( cet_set_perllib_directory )
  #message( STATUS "cet_make: cet_ups_dir is ${cet_ups_dir}")
  execute_process(COMMAND ${CET_REPORT} perllib ${cet_ups_dir}
    OUTPUT_VARIABLE REPORT_PERLLIB_MSG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
  #message( STATUS "${CET_REPORT} perllib returned ${REPORT_PERLLIB_MSG}")
  if( ${REPORT_PERLLIB_MSG} MATCHES "DEFAULT" )
     set( ${product}_perllib "NONE" CACHE STRING "Package perllib directory" FORCE )
  elseif( ${REPORT_PERLLIB_MSG} MATCHES "NONE" )
     set( ${product}_perllib ${REPORT_PERLLIB_MSG} CACHE STRING "Package perllib directory" FORCE )
  elseif( ${REPORT_PERLLIB_MSG} MATCHES "ERROR" )
     set( ${product}_perllib ${REPORT_PERLLIB_MSG} CACHE STRING "Package perllib directory" FORCE )
  else()
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" fdir1 "${REPORT_PERLLIB_MSG}" )
    STRING( REGEX REPLACE "product_dir" "${product}/${version}" fdir2 "${fdir1}" )
    set( ${product}_perllib ${fdir2}  CACHE STRING "Package perllib directory" FORCE )
    set( ${product}_ups_perllib ${REPORT_PERLLIB_MSG}  CACHE STRING "Package perllib ups directory" FORCE )
    get_filename_component( ${product}_perllib_subdir "${REPORT_PERLLIB_MSG}" NAME CACHE STRING "Package perllib subdirectory" FORCE)
  endif()
  #message( STATUS "cet_set_perllib_directory: ${product}_perllib is ${${product}_perllib}")
  #message( STATUS "cet_set_perllib_directory: ${product}_perllib_subdir is ${${product}_perllib_subdir}")
endmacro( cet_set_perllib_directory )

macro( cet_set_wp_directory )
  execute_process(COMMAND ${CET_REPORT} wpdir ${cet_ups_dir}
    OUTPUT_VARIABLE REPORT_WP_DIR_MSG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
  #message( STATUS "${CET_REPORT} wpdir returned ${REPORT_WP_DIR_MSG}")
  if( ${REPORT_WP_DIR_MSG} MATCHES "DEFAULT" )
     set( ${product}_wp_dir "NONE" CACHE STRING "Package wp directory" FORCE )
  elseif( ${REPORT_WP_DIR_MSG} MATCHES "NONE" )
     set( ${product}_wp_dir ${REPORT_WP_DIR_MSG} CACHE STRING "Package wp directory" FORCE )
  elseif( ${REPORT_WP_DIR_MSG} MATCHES "ERROR" )
     set( ${product}_wp_dir ${REPORT_WP_DIR_MSG} CACHE STRING "Package wp directory" FORCE )
  else()
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" fdir1 "${REPORT_WP_DIR_MSG}" )
    STRING( REGEX REPLACE "product_dir" "${product}/${version}" fdir2 "${fdir1}" )
    set( ${product}_wp_dir ${fdir2}  CACHE STRING "Package wp directory" FORCE )
  endif()
  #message( STATUS "cet_set_wp_directory: ${product}_wp_dir is ${${product}_wp_dir}")
endmacro( cet_set_wp_directory )

macro( cet_set_inc_directory )
  execute_process(COMMAND ${CET_REPORT} incdir ${cet_ups_dir}
    OUTPUT_VARIABLE REPORT_INC_DIR_MSG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
  #message( STATUS "${CET_REPORT} incdir returned ${REPORT_INC_DIR_MSG}")
  if( ${REPORT_INC_DIR_MSG} MATCHES "DEFAULT" )
     set( ${product}_inc_dir "${product}/${version}/include" CACHE STRING "Package include directory" FORCE )
  elseif( ${REPORT_INC_DIR_MSG} MATCHES "NONE" )
     set( ${product}_inc_dir ${REPORT_INC_DIR_MSG} CACHE STRING "Package include directory" FORCE )
  elseif( ${REPORT_INC_DIR_MSG} MATCHES "ERROR" )
     set( ${product}_inc_dir ${REPORT_INC_DIR_MSG} CACHE STRING "Package include directory" FORCE )
  else()
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" ldir1 "${REPORT_INC_DIR_MSG}" )
    STRING( REGEX REPLACE "product_dir" "${product}/${version}" ldir2 "${ldir1}" )
    set( ${product}_inc_dir ${ldir2}  CACHE STRING "Package include directory" FORCE )
  endif()
  #message( STATUS "cet_set_inc_directory: ${product}_inc_dir is ${${product}_inc_dir}")
endmacro( cet_set_inc_directory )

macro( cet_set_test_directory )
  # The default is product_dir/test
  #message( STATUS "cet_make: cet_ups_dir is ${cet_ups_dir}")
  execute_process(COMMAND ${CET_REPORT} testdir ${cet_ups_dir}
    OUTPUT_VARIABLE REPORT_TEST_DIR_MSG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
  #message( STATUS "${CET_REPORT} testdir returned ${REPORT_TEST_DIR_MSG}")
  if( ${REPORT_TEST_DIR_MSG} MATCHES "DEFAULT" )
     set( ${product}_test_dir ${product}/${version}/test CACHE STRING "Package test directory" FORCE )
  elseif( ${REPORT_TEST_DIR_MSG} MATCHES "NONE" )
     set( ${product}_test_dir ${REPORT_TEST_DIR_MSG} CACHE STRING "Package test directory" FORCE )
  elseif( ${REPORT_TEST_DIR_MSG} MATCHES "ERROR" )
     set( ${product}_test_dir ${REPORT_TEST_DIR_MSG} CACHE STRING "Package test directory" FORCE )
  else()
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" bdir1 "${REPORT_TEST_DIR_MSG}" )
    STRING( REGEX REPLACE "product_dir" "${product}/${version}" bdir2 "${bdir1}" )
    set( ${product}_test_dir ${bdir2}  CACHE STRING "Package test directory" FORCE )
  endif()
  #message( STATUS "cet_set_test_directory: ${product}_test_dir is ${${product}_test_dir}")
endmacro( cet_set_test_directory )

macro(_cet_debug_message)
  string(TOUPPER "${CMAKE_BUILD_TYPE}" BTYPE_UC )
  if( BTYPE_UC STREQUAL  "DEBUG" )
    message( STATUS "${ARGN}")
  endif()
endmacro(_cet_debug_message)

macro( set_install_root )
  set( PACKAGE_TOP_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  message( STATUS "set_install_root: PACKAGE_TOP_DIRECTORY is ${PACKAGE_TOP_DIRECTORY}")
endmacro( set_install_root )
