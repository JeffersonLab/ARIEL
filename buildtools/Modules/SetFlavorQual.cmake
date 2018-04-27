# determine the system flavor and define flavorqual_dir
#
# set_flavor_qual( [arch] )
#   noop - no longer used
#
# process_ups_files()
#   noop - no longer used

macro( set_flavor_qual )
message(STATUS "Building for ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}" )
endmacro( set_flavor_qual )

macro( process_ups_files )
endmacro( process_ups_files )
