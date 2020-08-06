# define the environment for a ups product
#
# find_ups_product( PRODUCTNAME [version] )
#  PRODUCTNAME - product name
#  version - optional minimum version
#
# cet_cmake_config() will put ${PRODUCTNAME}Config.cmake in
#  ${flavorqual_dir}/lib/cmake/${PRODUCTNAME} or share/${PRODUCTNAME}/cmake
# check these directories for allowed cmake configure files:
# either ${PRODUCTNAME_LC}-config.cmake  or ${PRODUCTNAME}Config.cmake
# call find_package() if we find the config file
# if the config file is not found, call _check_version()

include(CheckUpsVersion)

macro( _use_find_package PNAME )

cmake_parse_arguments( UFP "" "" "" ${ARGN} )
#message ( STATUS "_use_find_package debug: unparsed arguments ${UFP_UNPARSED_ARGUMENTS}" )
set( dotver "" )
if( UFP_UNPARSED_ARGUMENTS )
  list( GET UFP_UNPARSED_ARGUMENTS 0 PVER )
  _get_dotver( ${PVER} )
endif()
#message ( STATUS "_use_find_package debug: called with ${PNAME} ${PVER}" )
#message(STATUS "_use_find_package: dotver is ${dotver}")
#message(STATUS "_use_find_package: looking for ${PNAME} ${dotver}")

# Map product name "sqlite" to "sqlite3"
string(REGEX REPLACE "sqlite( |$)" "SQLite3" RNAME ${PNAME})
string(REGEX REPLACE "(^| )tbb( |$)" "TBB" RNAME ${RNAME})
string(REGEX REPLACE "(^| )clhep( |$)" "CLHEP" RNAME ${RNAME})
# find_package(Python2...) requires CMake 3.12
string(REGEX REPLACE "(^| )python( |$)" "Python2" RNAME ${RNAME})
#message( STATUS "Searching for ${RNAME}" )
if( ${RNAME} STREQUAL "Python2" )
  set(comp "COMPONENTS;Interpreter;Development")
endif()

# use find_package to check the version
# assume the package's ${RNAME}Config.cmake will set up include_diretcories,
# link libraries and various CMake variables for use with this project's CMakeLists.txt
find_package(${RNAME} ${dotver} ${comp} QUIET)
unset(comp)

if( NOT ${${RNAME}_FOUND} )
  # If not found, try pkg-config
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    # UPS "python" is currently v2.7, which is "python2" in pkg-config land
    string(REGEX REPLACE "(^| )python( |$)" "python2" RNAME ${RNAME})
    if(${dotver})
      pkg_check_modules(PC_${RNAME} ${RNAME}>=${dotver})
    else()
      pkg_check_modules(PC_${RNAME} ${RNAME})
    endif()
    if(PC_${RNAME}_FOUND)
      if(PC_${RNAME}_INCLUDE_DIRS)
#      message(STATUS "_use_find_package: adding ${PC_${RNAME}_INCLUDE_DIRS} to include_directories")
        include_directories(BEFORE ${PC_${RNAME}_INCLUDE_DIRS})
      endif()
      if(PC_${RNAME}_INCLUDEDIR)
#        message(STATUS "_use_find_package: adding ${PC_${RNAME}_INCLUDEDIR} to include_directories")
        include_directories(BEFORE ${PC_${RNAME}_INCLUDEDIR})
      endif()
      if(PC_${RNAME}_LIBRARY_DIRS)
#        message(STATUS "_use_find_package: adding ${PC_${RNAME}_LIBRARY_DIRS} to link_directories")
#        link_directories(${PC_${RNAME}_LIBRARY_DIRS})
        set(CMAKE_LIBRARY_PATH "${PC_${RNAME}_LIBRARY_DIRS}" "${CMAKE_LIBRARY_PATH}")
      endif()
      if(PC_${RNAME}_LIBDIR)
#        message(STATUS "_use_find_package: adding ${PC_${RNAME}_LIBDIR} to link_directories")
#        link_directories(${PC_${RNAME}_LIBDIR})
        set(CMAKE_LIBRARY_PATH "${PC_${RNAME}_LIBDIR}" "${CMAKE_LIBRARY_PATH}")
      endif()
    else()
      message(FATAL_ERROR "${PNAME} was NOT found ")
    endif()
  endif()
endif()
set( ${RNAME}_DOT_VERSION ${${RNAME}_VERSION})

endmacro( _use_find_package )

# since variables are passed, this is implemented as a macro
macro( find_ups_product PRODUCTNAME )

# if ${PRODUCTNAME}_UPS_VERSION is already defined, there is nothing to do
if( ${PRODUCTNAME}_UPS_VERSION )
  ##message( STATUS "find_ups_product debug: ${PRODUCTNAME}_UPS_VERSION ${${PRODUCTNAME}_UPS_VERSION} is defined" )
   else()
  ##message( STATUS "find_ups_product debug: ${PRODUCTNAME}_UPS_VERSION ${${PRODUCTNAME}_UPS_VERSION} is NOT defined" )
  cmake_parse_arguments( FUP "" "" "" ${ARGN} )
  #message ( STATUS "find_ups_product debug: unparsed arguments ${FUP_UNPARSED_ARGUMENTS}" )
  set( fup_version "" )
  if( FUP_UNPARSED_ARGUMENTS )
    list( GET FUP_UNPARSED_ARGUMENTS 0 fup_version )
  endif()
  #message ( STATUS "find_ups_product debug: called with ${PRODUCTNAME} ${fup_version}" )

  #message(STATUS "find_ups_product debug: ${PRODUCTNAME} ${cet_product_list}")
  list(FIND cet_product_list ${PRODUCTNAME} found_product_match)

  if( ${PRODUCTNAME} MATCHES "cetbuildtools" )
  elseif( ${found_product_match} LESS 0 )
    #message(STATUS "find_ups_product debug: ${found_product_match} for ${PRODUCTNAME} ")
    # add to product list
    ##set(CONFIG_FIND_UPS_COMMANDS "${CONFIG_FIND_UPS_COMMANDS}
    ##find_ups_product( ${PRODUCTNAME} ${fup_version} )")
    list(APPEND CONFIG_FIND_LIBRARY_COMMAND_LIST "find_ups_product( ${PRODUCTNAME} ${fup_version} )")
    set(cet_product_list ${PRODUCTNAME} ${cet_product_list} )
    #message(STATUS "find_ups_product debug: adding find_ups_product( ${PRODUCTNAME} ${fup_version} )")
    #_cet_debug_message("find_ups_product: ${PRODUCTNAME} version is ${${${PRODUCTNAME}_UC}_VERSION} ")
  endif()

  _use_find_package( ${PRODUCTNAME} ${fup_version} )

endif()

endmacro( find_ups_product )
