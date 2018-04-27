# build the ups table file from information in product_deps
#
# cet_build_table()
#

macro( cet_build_table )
   find_program(BUILD_TABLE_NAME build_table "${cetbuildtools_BINDIR}")
   if ( NOT BUILD_TABLE_NAME )
     MESSAGE(FATAL_ERROR "cet_version_file: could not find build_table in ${cetbuildtools_BINDIR}")
   endif ( NOT BUILD_TABLE_NAME )

   cet_base_flags()
   # make a temporary file with the variables
   file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/cet_base_flags
"CET_BASE_CXX_FLAG_DEBUG: ${CET_BASE_CXX_FLAG_DEBUG}
CET_BASE_CXX_FLAG_OPT:   ${CET_BASE_CXX_FLAG_OPT}
CET_BASE_CXX_FLAG_PROF:  ${CET_BASE_CXX_FLAG_PROF}
CET_BASE_C_FLAG_DEBUG:   ${CET_BASE_C_FLAG_DEBUG}
CET_BASE_C_FLAG_OPT:     ${CET_BASE_C_FLAG_OPT}
CET_BASE_C_FLAG_PROF:    ${CET_BASE_C_FLAG_PROF}
" )

   ##message( STATUS "cet_build_table: cet_ups_dir is ${cet_ups_dir}")
   execute_process(COMMAND ${BUILD_TABLE_NAME}
     ${cet_ups_dir}
		 ${CMAKE_CURRENT_BINARY_DIR}
     OUTPUT_VARIABLE MSG
		 OUTPUT_STRIP_TRAILING_WHITESPACE
		 )
   ##message( STATUS "${BUILD_TABLE_NAME} returned ${MSG}")
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${product}.table
           DESTINATION ${product}/${version}/ups )

endmacro( cet_build_table )


macro( cet_version_file )
   find_program(BUILD_VERSION_FILE_NAME build_version_file "${cetbuildtools_BINDIR}")
   if ( NOT BUILD_VERSION_FILE_NAME )
     MESSAGE(FATAL_ERROR "cet_version_file: could not find build_version_file in ${cetbuildtools_BINDIR}")
   endif ( NOT BUILD_VERSION_FILE_NAME )

   if ( ${qualifier} MATCHES "-nq-" )
     set( VQUAL "" )
     set( FQUAL "" )
   else ()
     STRING( REGEX REPLACE ":" "_" VQUAL "${${product}_full_qualifier}" )
     set( FQUAL ${${product}_full_qualifier} )
   endif()
   ##message( STATUS "calling ${BUILD_VERSION_FILE_NAME} with ${CMAKE_CURRENT_BINARY_DIR}/${UPSFLAVOR}_${VQUAL} ${product} ${version} ${default_version} ${UPSFLAVOR} ${FQUAL}")
   execute_process(COMMAND ${BUILD_VERSION_FILE_NAME}
		 ${CMAKE_CURRENT_BINARY_DIR}/${UPSFLAVOR}_${VQUAL}
		 ${product}
		 ${version}
		 ${default_version}
		 ${UPSFLAVOR}
		 ${FQUAL}
     OUTPUT_VARIABLE MSG
		 OUTPUT_STRIP_TRAILING_WHITESPACE
		 )
   ##message( STATUS "${BUILD_VERSION_FILE_NAME} returned ${MSG}")
   # check to see if we have a current chain
   if( ${default_version} MATCHES "current" )
      install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${UPSFLAVOR}_${VQUAL}
               DESTINATION ${product}/${version}.version )
      install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${UPSFLAVOR}
               DESTINATION ${product}/current.chain )
   else()
      install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${UPSFLAVOR}_${VQUAL}
               DESTINATION ${product}/${version}.version )
   endif()

endmacro( cet_version_file )
