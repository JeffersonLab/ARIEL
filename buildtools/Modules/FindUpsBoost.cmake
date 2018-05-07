# Find Boost libraries
#
# find_ups_boost(  [minimum] )
#  minimum - optional minimum version
#  we look for nearly all of the boost libraries
#      except math, prg_exec_monitor, test_exec_monitor

include(CheckUpsVersion)

# since variables are passed, this is implemented as a macro
macro( find_ups_boost )

cmake_parse_arguments( FUB "" "" "" ${ARGN} )
set( minimum )
if( FUB_UNPARSED_ARGUMENTS )
  list( GET FUB_UNPARSED_ARGUMENTS 0 minimum )
endif()

set(boost_liblist chrono 
                  date_time  
		  filesystem 
		  graph
		  iostreams
		  locale
		  prg_exec_monitor
		  program_options 
		  random
		  regex 
		  serialization
		  signals
		  system 
		  thread 
		  timer
		  unit_test_framework 
		  wave
		  wserialization )

# compare for recursion
list(FIND cet_product_list boost found_product_match)
if( ${found_product_match} LESS 0 )
  # add to product list
  set(CONFIG_FIND_UPS_COMMANDS "${CONFIG_FIND_UPS_COMMANDS}
    find_ups_boost( ${minimum} )")
  set(cet_product_list boost ${cet_product_list} )

  # Hint at library location - if Boost is available in our software environment,
  # prefer it over the system libraries
  set(BOOST_ROOT $ENV{JERM_ROOT})

  # Bes ure to find the threaed-safe libraries
  set(Boost_USE_MULTITHREADED ON)

  # Disable loading of possible CMake configuration files - their behavior may not be standardized

  set(Boost_NO_BOOST_CMAKE ON)
  #set(Boost_ADDITIONAL_VERSIONS "1.48" "1.48.0" "1.49" "1.49.0")

  # Translate vx_y_z to x.y.z - minimum acceptable library version (if any)
  if( ${minimum} )
    _get_dotver( ${minimum} )
  endif()

  # Find the package (using CMake's FindBoost)
  find_package( Boost ${dotver} COMPONENTS ${boost_liblist} REQUIRED )

  # Build a UPS-style vx_y_z version string - the actual found version of Boost
  string(CONCAT BOOST_VERS "v" ${Boost_MAJOR_VERSION} "_" ${Boost_MINOR_VERSION} "_" ${Boost_SUBMINOR_VERSION})
  
  #message(STATUS "find_ups_boost debug: Boost version is ${BOOST_VERS}" )
  #message(STATUS "find_ups_boost debug: Boost include directory is ${Boost_INCLUDE_DIRS}" )
  #message(STATUS "find_ups_boost debug: Boost library directory is ${Boost_LIBRARY_DIRS}" )
  #message(STATUS "find_ups_boost debug: Boost_FILESYSTEM_LIBRARY is ${Boost_FILESYSTEM_LIBRARY}" )

  include_directories ( SYSTEM ${Boost_INCLUDE_DIRS} )

endif()

endmacro( find_ups_boost )
