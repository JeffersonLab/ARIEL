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
include(CetHaveQual)  # Needed by product CMakeLists.txt

macro(_get_cetpkg_info)
  cet_get_product_info_item(product rproduct ec_product)
  if(ec_product)
    message(FATAL_ERROR "Unable to obtain product information: need to re-source setup_for_development?")
  endif()

  cet_get_product_info_item(version rversion)

  set(product ${rproduct} CACHE STRING "Package name" FORCE)
  set(version ${rversion} CACHE STRING "Package version" FORCE)
  #message(STATUS "_get_cetpkg_info: found ${product} ${version}")

  set( cet_ups_dir ${CMAKE_CURRENT_SOURCE_DIR}/ups CACHE STRING "Package UPS directory" FORCE )
  #message( STATUS "_get_cetpkg_info: cet_ups_dir is ${cet_ups_dir}")
endmacro(_get_cetpkg_info)

macro(cet_cmake_env)
  # project() must have been called before us.
  if(NOT CMAKE_PROJECT_NAME)
    message (FATAL_ERROR
      "CMake project() command must have been invoked prior to cet_cmake_env()."
      "\nIt must be invoked at the top level, not in an included .cmake file.")
  endif()

  _get_cetpkg_info()
  
  # temporarily set this policy
  # silently ignore non-existent dependencies
  cmake_policy(SET CMP0046 OLD)

  # Acknowledge new RPATH behavior on OS X.
  cmake_policy(SET CMP0042 NEW)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
  endif()

  message(STATUS "Product is ${product} ${version}")
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
  include(InstallSource)
  include(InstallLicense)
  include(InstallFiles)
  include(InstallPerllib)
  include(CetCMakeUtils)
  include(CetMake)
  include(CetCMakeConfig)

  # initialize cmake config file fragments
  _cet_init_config_var()

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
  #define flavorqual_dir
  set_flavor_qual( ${arch} )
  cet_set_lib_directory()
  cet_set_bin_directory()
  cet_set_inc_directory()
  cet_set_fcl_directory()
  cet_set_data_directory()
  cet_set_fw_directory()
  cet_set_gdml_directory()
  cet_set_perllib_directory()
  cet_set_test_directory()

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

macro(_expand_std_dirs REPORT_MSG OUT_VAR)
  if(flavorqual_dir)
    STRING( REGEX REPLACE "flavorqual_dir" "${flavorqual_dir}" _dir1 "${REPORT_MSG}" )
  else()
    STRING( REGEX REPLACE "flavorqual_dir/?" "" _dir1 "${REPORT_MSG}" )
  endif()
  STRING( REGEX REPLACE "product_dir/?" "" _dir2 "${_dir1}" )
  set(${OUT_VAR} "${_dir2}")
endmacro(_expand_std_dirs)

# Set installation directories
# (at least for binaries/data - see InstallSource.cmake for additional definitions)
# Any JERM_INSTALL_<TYPE>DIR definitions override, otherwise use results from cet_report,
# which scans the product_deps file for the package author's preferences
macro( cet_set_lib_directory )
  if (JERM_INSTALL_LIBDIR)
    set( ${product}_lib_dir ${JERM_INSTALL_LIBDIR} CACHE STRING "Package lib directory" FORCE )
  else()
    execute_process(COMMAND ${CET_REPORT} libdir ${cet_ups_dir}
      OUTPUT_VARIABLE REPORT_LIB_DIR_MSG
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    #message( STATUS "${CET_REPORT} libdir returned ${REPORT_LIB_DIR_MSG}")
    if( ${REPORT_LIB_DIR_MSG} MATCHES "DEFAULT" )
      _expand_std_dirs( "flavorqual_dir/lib" ldir )
      set( ${product}_lib_dir ${ldir} CACHE STRING "Package lib directory" FORCE )
    elseif( ${REPORT_LIB_DIR_MSG} MATCHES "NONE" )
      set( ${product}_lib_dir ${REPORT_LIB_DIR_MSG} CACHE STRING "Package lib directory" FORCE )
    elseif( ${REPORT_LIB_DIR_MSG} MATCHES "ERROR" )
      set( ${product}_lib_dir ${REPORT_LIB_DIR_MSG} CACHE STRING "Package lib directory" FORCE )
    else()
      _expand_std_dirs( "${REPORT_LIB_DIR_MSG}" ldir )
      set( ${product}_lib_dir ${ldir}  CACHE STRING "Package lib directory" FORCE )
    endif()
  endif()
  message( STATUS "cet_set_lib_directory: ${product}_lib_dir is ${${product}_lib_dir}")
endmacro( cet_set_lib_directory )

macro( cet_set_bin_directory )
  if (JERM_INSTALL_BINDIR)
    set( ${product}_bin_dir ${JERM_INSTALL_BINDIR} CACHE STRING "Package bin directory" FORCE )
  else()
    execute_process(COMMAND ${CET_REPORT} bindir ${cet_ups_dir}
      OUTPUT_VARIABLE REPORT_BIN_DIR_MSG
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    #message( STATUS "${CET_REPORT} bindir returned ${REPORT_BIN_DIR_MSG}")
    if( ${REPORT_BIN_DIR_MSG} MATCHES "DEFAULT" )
      _expand_std_dirs( "flavorqual_dir/bin" bdir )
      set( ${product}_bin_dir ${bdir} CACHE STRING "Package bin directory" FORCE )
    elseif( ${REPORT_BIN_DIR_MSG} MATCHES "NONE" )
      set( ${product}_bin_dir ${REPORT_BIN_DIR_MSG} CACHE STRING "Package bin directory" FORCE )
    elseif( ${REPORT_BIN_DIR_MSG} MATCHES "ERROR" )
      set( ${product}_bin_dir ${REPORT_BIN_DIR_MSG} CACHE STRING "Package bin directory" FORCE )
    else()
      _expand_std_dirs( "${REPORT_BIN_DIR_MSG}" bdir )
      set( ${product}_bin_dir ${bdir}  CACHE STRING "Package bin directory" FORCE )
    endif()
  endif()
  message( STATUS "cet_set_bin_directory: ${product}_bin_dir is ${${product}_bin_dir}")
endmacro( cet_set_bin_directory )

macro( cet_set_fcl_directory )
  if (JERM_INSTALL_FCLDIR)
    set( ${product}_fcl_dir ${JERM_INSTALL_FCLDIR} CACHE STRING "Package fcl directory" FORCE )
  elseif (JERM_INSTALL_DATADIR)
    set( ${product}_fcl_dir ${JERM_INSTALL_DATADIR}/fcl CACHE STRING "Package fcl directory" FORCE )
  else()
    execute_process(COMMAND ${CET_REPORT} fcldir ${cet_ups_dir}
      OUTPUT_VARIABLE REPORT_FCL_DIR_MSG
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    #message( STATUS "${CET_REPORT} fcldir returned ${REPORT_FCL_DIR_MSG}")
    if( ${REPORT_FCL_DIR_MSG} MATCHES "DEFAULT" )
      set( ${product}_fcl_dir share/${product}/fcl CACHE STRING "Package fcl directory" FORCE )
    elseif( ${REPORT_FCL_DIR_MSG} MATCHES "NONE" )
      set( ${product}_fcl_dir ${REPORT_FCL_DIR_MSG} CACHE STRING "Package fcl directory" FORCE )
    elseif( ${REPORT_FCL_DIR_MSG} MATCHES "ERROR" )
      set( ${product}_fcl_dir ${REPORT_FCL_DIR_MSG} CACHE STRING "Package fcl directory" FORCE )
    else()
      _expand_std_dirs( "${REPORT_FCL_DIR_MSG}" fdir )
      set( ${product}_fcl_dir ${fdir}  CACHE STRING "Package fcl directory" FORCE )
    endif()
  endif()
  message( STATUS "cet_set_fcl_directory: ${product}_fcl_dir is ${${product}_fcl_dir}")
endmacro( cet_set_fcl_directory )

macro( cet_set_data_directory )
  if (JERM_INSTALL_DATADIR)
    set( ${product}_data_dir ${JERM_INSTALL_DATADIR} CACHE STRING "Package data directory" FORCE )
  else()
    set( ${product}_data_dir share/${product} CACHE STRING "Package data directory" FORCE )
  endif()
  message( STATUS "cet_set_data_directory: ${product}_data_dir is ${${product}_data_dir}")
endmacro( cet_set_data_directory )

macro( cet_set_fw_directory )
  if (JERM_INSTALL_FWDIR)
    set( ${product}_fw_dir ${JERM_INSTALL_FWDIR} CACHE STRING "Package fw directory" FORCE )
  else()
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
      _expand_std_dirs( "${REPORT_FW_DIR_MSG}" fdir )
      set( ${product}_fw_dir ${fdir}  CACHE STRING "Package fw directory" FORCE )
    endif()
  endif()
  message( STATUS "cet_set_fw_directory: ${product}_fw_dir is ${${product}_fw_dir}")
endmacro( cet_set_fw_directory )

macro( cet_set_gdml_directory )
  if (JERM_INSTALL_GDMLDIR)
    set( ${product}_gdml_dir ${JERM_INSTALL_GDMLDIR} CACHE STRING "Package gdml directory" FORCE )
  else()
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
      _expand_std_dirs( "${REPORT_GDML_DIR_MSG}" gdir )
      set( ${product}_gdml_dir ${gdir}  CACHE STRING "Package gdml directory" FORCE )
    endif()
  endif()
  message( STATUS "cet_set_gdml_directory: ${product}_gdml_dir is ${${product}_gdml_dir}")
endmacro( cet_set_gdml_directory )

macro( cet_set_perllib_directory )
  if (JERM_INSTALL_PERLLIBDIR)
    set( ${product}_perllib ${JERM_INSTALL_PERLLIBDIR} CACHE STRING "Package perllib directory" FORCE )
  else()
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
      _expand_std_dirs( "${REPORT_PERLLIB_MSG}" pdir )
      set( ${product}_perllib ${pdir}  CACHE STRING "Package perllib directory" FORCE )
      get_filename_component( ${product}_perllib_subdir "${REPORT_PERLLIB_MSG}" NAME CACHE STRING "Package perllib subdirectory" FORCE)
    endif()
  endif()
  message( STATUS "cet_set_perllib_directory: ${product}_perllib is ${${product}_perllib}")
  message( STATUS "cet_set_perllib_directory: ${product}_perllib_subdir is ${${product}_perllib_subdir}")
endmacro( cet_set_perllib_directory )

macro( cet_set_inc_directory )
  if (JERM_INSTALL_INCLUDEDIR)
    set( ${product}_inc_dir ${JERM_INSTALL_INCLUDEDIR} CACHE STRING "Package include directory" FORCE )
  else()
    execute_process(COMMAND ${CET_REPORT} incdir ${cet_ups_dir}
      OUTPUT_VARIABLE REPORT_INC_DIR_MSG
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    #message( STATUS "${CET_REPORT} incdir returned ${REPORT_INC_DIR_MSG}")
    if( ${REPORT_INC_DIR_MSG} MATCHES "DEFAULT" )
      set( ${product}_inc_dir "include" CACHE STRING "Package include directory" FORCE )
    elseif( ${REPORT_INC_DIR_MSG} MATCHES "NONE" )
      set( ${product}_inc_dir ${REPORT_INC_DIR_MSG} CACHE STRING "Package include directory" FORCE )
    elseif( ${REPORT_INC_DIR_MSG} MATCHES "ERROR" )
      set( ${product}_inc_dir ${REPORT_INC_DIR_MSG} CACHE STRING "Package include directory" FORCE )
    else()
      _expand_std_dirs( "${REPORT_INC_DIR_MSG}" idir )
      set( ${product}_inc_dir ${idir}  CACHE STRING "Package include directory" FORCE )
    endif()
  endif()
  message( STATUS "cet_set_inc_directory: ${product}_inc_dir is ${${product}_inc_dir}")
endmacro( cet_set_inc_directory )

macro( cet_set_test_directory )
  # The default is share/product_dir/test
  if (JERM_INSTALL_TESTDIR)
    set( ${product}_test_dir ${JERM_INSTALL_TESTDIR} CACHE STRING "Package test directory" FORCE )
  else()
    execute_process(COMMAND ${CET_REPORT} testdir ${cet_ups_dir}
      OUTPUT_VARIABLE REPORT_TEST_DIR_MSG
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    #message( STATUS "${CET_REPORT} testdir returned ${REPORT_TEST_DIR_MSG}")
    if( ${REPORT_TEST_DIR_MSG} MATCHES "DEFAULT" )
      set( ${product}_test_dir share/${product}/test CACHE STRING "Package test directory" FORCE )
    elseif( ${REPORT_TEST_DIR_MSG} MATCHES "NONE" )
      set( ${product}_test_dir ${REPORT_TEST_DIR_MSG} CACHE STRING "Package test directory" FORCE )
    elseif( ${REPORT_TEST_DIR_MSG} MATCHES "ERROR" )
      set( ${product}_test_dir ${REPORT_TEST_DIR_MSG} CACHE STRING "Package test directory" FORCE )
    else()
      _expand_std_dirs( "${REPORT_TEST_DIR_MSG}" tdir )
      set( ${product}_test_dir ${tdir}  CACHE STRING "Package test directory" FORCE )
    endif()
  endif()
  message( STATUS "cet_set_test_directory: ${product}_test_dir is ${${product}_test_dir}")
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
