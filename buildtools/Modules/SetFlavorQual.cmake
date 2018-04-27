# determine the system flavor and define flavorqual_dir
#
# set_flavor_qual( [arch] )
# allow optional architecture declaration
# noarch is recognized, others are used at your own discretion
#
# process_ups_files()
#   the configure and install steps for ups version and table files

include(CetBuildTable)

macro( set_flavor_qual )

set(arch "${ARGN}")

FIND_PROGRAM( CETB_GET_DIRECTORY_NAME get-directory-name )
set( UPS_DIR $ENV{UPS_DIR} )
set( THIS_PLATFORM ${CMAKE_SYSTEM_PROCESSOR} )

# sl5 is 2.6.18
# sl4 is 2.6.9
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
   # set OSTYPE
   execute_process(COMMAND ${CETB_GET_DIRECTORY_NAME} os 
                   OUTPUT_VARIABLE OSTYPE
		   OUTPUT_STRIP_TRAILING_WHITESPACE
		   )
   # find ups flavor
   if( NOT UPS_DIR )
       if( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "^2.6.9.*" )
	   if( ${THIS_PLATFORM} MATCHES "x86_64" )
               set( UPSFLAVOR Linux64bit+2.6-2.3.4)
	   elseif( ${THIS_PLATFORM} MATCHES "ppc" )
               set( UPSFLAVOR Linuxppc+2.6-2.3.4)
	   else ()
               set( UPSFLAVOR Linux+2.6-2.3.4)
	   endif ()
       endif ()
       if( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "^2.6.18.*" )
	   if( ${THIS_PLATFORM} MATCHES "x86_64" )
               set( UPSFLAVOR Linux64bit+2.6-2.5)
	   elseif( ${THIS_PLATFORM} MATCHES "ppc" )
               set( UPSFLAVOR Linuxppc+2.6-2.5)
	   else ()
               set( UPSFLAVOR Linux+2.6-2.5)
	   endif ()
       endif ()
   else()
       FIND_PROGRAM( CET_UPS ups ${UPS_DIR}/bin  )
       execute_process(COMMAND ${CET_UPS} flavor -4
                       OUTPUT_VARIABLE UPSFLAVOR
		       OUTPUT_STRIP_TRAILING_WHITESPACE
		       )
       #message( STATUS " flavor is ${UPSFLAVOR}" )
   endif ()
   if( CMAKE_CROSSCOMPILING )
       if( ${THIS_PLATFORM} MATCHES "ppc" )
	   if( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "^2.6.9.*" )
        	   set( UPSFLAVOR Linuxppc+2.6-2.3.4)
	   elseif( ${CMAKE_HOST_SYSTEM_VERSION} MATCHES "^2.6.18.*" )
        	   set( UPSFLAVOR Linuxppc+2.6-2.5)
	   else ()
        	   set( UPSFLAVOR Linuxppc)
	   endif ()
       endif()
   endif ()
endif ()
# CHG 2012/06/22 - Changed this since uname now produces Darwin version
# for Snow Leopard and (hopefully) later.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
   STRING(REGEX MATCH "^([0-9]+)" DARWIN_VERSION "${CMAKE_HOST_SYSTEM_VERSION}")
   set( OSTYPE "d${DARWIN_VERSION}" )
   if( NOT UPS_DIR )
       set( UPSFLAVOR "Darwin+${DARWIN_VERSION}" )
   else()
       FIND_PROGRAM( CET_UPS ups ${UPS_DIR}/bin  )
       execute_process(COMMAND ${CET_UPS} flavor -2
                       OUTPUT_VARIABLE UPSFLAVOR
		       OUTPUT_STRIP_TRAILING_WHITESPACE
		       )
   endif ()
   #message(STATUS "set_flavor_qual: flavor is ${UPSFLAVOR}")
   execute_process(COMMAND ${CETB_GET_DIRECTORY_NAME} platform
                   OUTPUT_VARIABLE THIS_PLATFORM 
		   OUTPUT_STRIP_TRAILING_WHITESPACE
		   )
   #message(STATUS "set_flavor_qual: THIS_PLATFORM ${THIS_PLATFORM}")
endif ()
message(STATUS "Building for ${CMAKE_SYSTEM_NAME} ${OSTYPE} ${THIS_PLATFORM}" )

if ( arch )
   set( OSTYPE ${arch} )
   set( UPSFLAVOR ${arch} )
   if( ${arch} MATCHES "noarch" )
       set( OSTYPE ${arch} )
       set( UPSFLAVOR NULL )
   endif ()
endif ()
# require OSTYPE
if ( NOT OSTYPE )
  message(FATAL_ERROR "Can't determine system type from ${CMAKE_HOST_SYSTEM_VERSION}")
endif ()

# all qualifiers are passed
STRING( REGEX REPLACE ":" "." QUAL_SUBDIR "${${product}_full_qualifier}" )
#message(STATUS "qualifiers: ${${product}_full_qualifier} ${QUAL_SUBDIR}")

if( ${arch} MATCHES "noarch" )
    SET (flavorqual ${OSTYPE}.${QUAL_SUBDIR} )
else ()
    SET (flavorqual ${OSTYPE}.${THIS_PLATFORM}.${QUAL_SUBDIR})
endif ()
SET (flavorqual_dir ${product}/${version}/${flavorqual} CACHE STRING "Flavor-qualified package install directory" FORCE)

#message(STATUS "set_flavor_qual: flavorqual is ${flavorqual}" )
#message(STATUS "set_flavor_qual: ups flavor is ${UPSFLAVOR}" )
#message(STATUS "set_flavor_qual: flavorqual directory is ${flavorqual_dir}" )

endmacro( set_flavor_qual )

macro( process_ups_files )
  if( NOT product )
    message(FATAL_ERROR "product name is not defined")
  endif()

  # table file
  cet_build_table()

  # version file
  cet_version_file()

endmacro( process_ups_files )
