# parse a ups version string and set the cmake project versions
#
# set_dot_version ( PRODUCTNAME UPS_VERSION )
# set_version_from_ups( UPS_VERSION )
# parse_ups_version( UPS_VERSION )
#  PRODUCTNAME - product name
#  UPS_VERSION - ups version of the form vx_y_z

macro( parse_ups_version UPS_VERSION )

  STRING(REGEX MATCHALL "_" ulist ${UPS_VERSION} ) 
  list( LENGTH ulist nunder )
  ##message(STATUS "parse_ups_version: ${UPS_VERSION} has ${nunder} underscores" )
  if ( ${nunder} STREQUAL 0 )
    STRING( REGEX REPLACE "^[v](.*)$" "\\1" VMAJ "${UPS_VERSION}" )
  elseif( ${nunder} STREQUAL 1 )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)$" "\\1" VMAJ "${UPS_VERSION}" )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)$" "\\2" VMIN "${UPS_VERSION}" )
  elseif( ${nunder} STREQUAL 2 )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)$" "\\1" VMAJ "${UPS_VERSION}" )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)$" "\\2" VMIN "${UPS_VERSION}" )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)$" "\\3" VPRJ "${UPS_VERSION}" )
  elseif( ${nunder} STREQUAL 3 )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)[_](.*)$" "\\1" VMAJ "${UPS_VERSION}" )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)[_](.*)$" "\\2" VMIN "${UPS_VERSION}" )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)[_](.*)$" "\\3" VPRJ "${UPS_VERSION}" )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)[_](.*)$" "\\4" VPT "${UPS_VERSION}" )
  else()
    message(STATUS "NOTE: ups version ${UPS_VERSION} has extra underscores")
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)[_](.*)$" "\\1" VMAJ "${UPS_VERSION}" )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)[_](.*)$" "\\2" VMIN "${UPS_VERSION}" )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)[_](.*)$" "\\3" VPRJ "${UPS_VERSION}" )
    STRING( REGEX REPLACE "^[v](.*)[_](.*)[_](.*)[_](.*)$" "\\4" VPT "${UPS_VERSION}" )
  endif()
  ##message(STATUS "parse_ups_version: version parses to ${VMAJ}.${VMIN}.${VPRJ}.${VPT}" )

endmacro( parse_ups_version )

macro( set_version_from_ups UPS_VERSION )

  parse_ups_version( ${UPS_VERSION} )

  set( VERSION_MAJOR ${VMAJ} CACHE STRING "Package major version" FORCE)
  set( VERSION_MINOR ${VMIN} CACHE STRING "Package minor version" FORCE )
  set( VERSION_PATCH ${VPRJ} CACHE STRING "Package patch version" FORCE )
  set( VERSION_TWEAK ${VPT} CACHE STRING "Package tweak version" FORCE )
  ##message(STATUS "set_version_from_ups: project version is ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_TWEAK}" )

endmacro( set_version_from_ups )

macro( set_dot_version PRODUCTNAME UPS_VERSION )

  string(TOUPPER  ${PRODUCTNAME} PRODUCTNAME_UC )
  STRING( REGEX REPLACE "_" "." VDOT "${UPS_VERSION}" )
  ##message(STATUS "temp version is ${VDOT}" )
  STRING( REGEX REPLACE "^[v]" "" ${PRODUCTNAME_UC}_DOT_VERSION "${VDOT}" )
  ##message(STATUS "set_dot_version: ${PRODUCTNAME_UC}_DOT_VERSION version is ${${PRODUCTNAME_UC}_DOT_VERSION}" )

endmacro( set_dot_version )
