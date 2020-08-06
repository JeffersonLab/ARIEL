# Find Boost libraries
#
# find_ups_boost([BOOST_TARGETS] [<min-ver>])
#
# BOOST_TARGETS
#   If this option is specified, the modern idiom of specifying Boost
#   libraries by target e.g. Boost::unit_test_framework should be
#   followed. Note that this idiom will work regardless for
#   now. Otherwise, a backward compatibilty option will be activated to
#   create Boost_XXXX_LIBRARY variables for use when linking.
#
#  <min-ver> - optional minimum version
#
# We look for nearly all of the boost libraries except math,
# prg_exec_monitor, test_exec_monitor
#
# If you need any that aren't specified in boost_liblist below, you
# should issue a separate CMake find_package(COMPONENTS ... REQUIRED)
# command subsequent to this one.

include(CheckUpsVersion)

set(boost_liblist
  chrono
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
	system
	thread
	timer
	unit_test_framework
	wave
	wserialization )

# since variables are passed, this is implemented as a macro
macro( find_ups_boost )

cmake_parse_arguments( FUB "BOOST_TARGETS" "" "" ${ARGN} )

if (NOT FUB_BOOST_TARGETS)
  set(Boost_NO_BOOST_CMAKE ON)
endif()

set( minimum )
if( FUB_UNPARSED_ARGUMENTS )
  list( GET FUB_UNPARSED_ARGUMENTS 0 minimum )
endif()

# compare for recursion
list(FIND cet_product_list boost found_product_match)
if( ${found_product_match} LESS 0 )
  # add to product list
  list(APPEND CONFIG_FIND_LIBRARY_COMMAND_LIST "find_ups_boost( ${minimum} )")
  set(cet_product_list boost ${cet_product_list} )

  # Hint at library location - if Boost is available in our software environment,
  # prefer it over the system libraries
  set(BOOST_ROOT $ENV{JERM_ROOT})

  # Be sure to find the thread-safe libraries
  set(Boost_USE_MULTITHREADED ON)

  # Translate vx_y_z to x.y.z - minimum acceptable library version (if any)
  if( minimum )
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
