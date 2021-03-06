# create the cmake configure files for this package
#
# cet_cmake_config( [NO_FLAVOR] )
#   build and install PackageConfig.cmake and PackageConfigVersion.cmake
#   these files are installed in ${flavorqual_dir}/lib/PACKAGE/cmake
#   if NO_FLAVOR is specified, the files are installed under ${product}/${version}/cmake

# this requires cmake 2.8.8 or later
include(CMakePackageConfigHelpers)

include(CetParseArgs)

function(_config_package_config_file)
  # Set variables (scope within this function only) with simpler names
  # for use inside package config files as e.g. @PACKAGE_bin_dir@
  foreach(path_type bin lib inc fcl gdml data)
    set(${path_type}_dir ${${product}_${path_type}_dir})
  endforeach()
  set(perllib ${${product}_perllib})
  string(REPLACE ";" "\n" CONFIG_FIND_UPS_COMMANDS "${CONFIG_FIND_UPS_COMMAND_LIST}")
  string(REPLACE ";" "\n" CONFIG_FIND_LIBRARY_COMMANDS "${CONFIG_FIND_LIBRARY_COMMAND_LIST}")
  configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/product-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${product}Config.cmake INSTALL_DESTINATION ${distdir}
    # Use known list of path vars for installation locations so these
    # can be found relative to the location of the productConfig.cmake
    # file
    PATH_VARS
    bin_dir
    lib_dir
    inc_dir
    fcl_dir
    gdml_dir
    data_dir
    perllib
    )
endfunction()

macro( cet_write_version_file _filename )

  cet_parse_args( CWV "VERSION;COMPATIBILITY" "" ${ARGN})

  find_file( versionTemplateFile
             NAMES CetBasicConfigVersion-${CWV_COMPATIBILITY}.cmake.in
             PATHS ${CMAKE_MODULE_PATH} )
  if(NOT EXISTS "${versionTemplateFile}")
    message(FATAL_ERROR "Bad COMPATIBILITY value used for cet_write_version_file(): \"${CWV_COMPATIBILITY}\"")
  endif()

  if("${CWV_VERSION}" STREQUAL "")
    message(FATAL_ERROR "No VERSION specified for cet_write_version_file()")
  endif()

  configure_file("${versionTemplateFile}" "${_filename}" @ONLY)
endmacro( cet_write_version_file )

macro( cet_cmake_config  )

  cet_parse_args( CCC "" "NO_FLAVOR" ${ARGN})

  if( CCC_NO_FLAVOR )
    set( distdir "${${product}_data_dir}/cmake" )
  else()
    set( distdir "${${product}_lib_dir}/cmake/${product}" )
  endif()

  #message(STATUS "cet_cmake_config debug: will install cmake configure files in ${distdir}")
  #message(STATUS "cet_cmake_config debug: ${CONFIG_FIND_UPS_COMMAND_LIST}")
  #message(STATUS "cet_cmake_config debug: ${CONFIG_FIND_LIBRARY_COMMAND_LIST}")
  #message(STATUS "cet_cmake_config debug: ${CONFIG_LIBRARY_LIST}")

  string(TOUPPER  ${product} ${product}_UC )
  # add to library list for package configure file
  foreach( my_library ${CONFIG_LIBRARY_LIST} )
    string(TOUPPER  ${my_library} ${my_library}_UC )
    string(TOUPPER  ${product} ${product}_UC )
    get_target_property(lib_type ${my_library} TYPE)
    if (lib_type STREQUAL "STATIC_LIBRARY")
      set(lib_basename ${CMAKE_STATIC_LIBRARY_PREFIX}${my_library}${CMAKE_STATIC_LIBRARY_SUFFIX})
    else()
      set(lib_basename ${CMAKE_SHARED_LIBRARY_PREFIX}${my_library}${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
    list(APPEND CONFIG_FIND_LIBRARY_COMMAND_LIST
      "set( ${${my_library}_UC} \"\${PACKAGE_PREFIX_DIR}/${${product}_lib_dir}/${lib_basename}\" )" )
    #cet_find_library( ${${my_library}_UC} NAMES ${my_library} PATHS ENV ${${product}_UC}_LIB NO_DEFAULT_PATH )" )
    ##message(STATUS "cet_cmake_config: cet_find_library( ${${my_library}_UC} NAMES ${my_library} PATHS ENV ${${product}_UC}_LIB NO_DEFAULT_PATH )" )
    ##message(STATUS "cet_cmake_config: set( ${${my_library}_UC}  \$ENV{${${product}_UC}_LIB}/lib${my_library}${CMAKE_SHARED_LIBRARY_SUFFIX} )" )
  endforeach(my_library)
  #message(STATUS "cet_cmake_config debug: ${CONFIG_FIND_LIBRARY_COMMAND_LIST}")

  # add include path to CONFIG_FIND_LIBRARY_COMMAND_LIST
  ##message(STATUS "cet_cmake_config: ${product}_inc_dir is ${${product}_inc_dir}")
  if( NOT ${${product}_inc_dir} MATCHES "NONE" )
    list(APPEND CONFIG_FIND_LIBRARY_COMMAND_LIST
      "include_directories ( \"\${PACKAGE_PREFIX_DIR}/${${product}_inc_dir}\" )" )
  endif()
  ##message(STATUS "cet_cmake_config: CONFIG_INCLUDE_DIRECTORY is ${CONFIG_INCLUDE_DIRECTORY}")

  # get perl library directory
  #message( STATUS "config_pm: ${product}_perllib is ${${product}_perllib}")
  #message( STATUS "config_pm: ${product}_ups_perllib is ${${product}_ups_perllib}")
  #message( STATUS "config_pm: ${product}_perllib_subdir is ${${product}_perllib_subdir}")
  set(mypmdir "\${PACKAGE_PREFIX_DIR}/${${product}_perllib}")
  #message( STATUS "config_pm: mypmdir ${mypmdir}")
  # PluginVersionInfo is a special case
  if( CONFIG_PM_VERSION )
    #message(STATUS "CONFIG_PM_VERSION is ${CONFIG_PM_VERSION}" )
    string(REGEX REPLACE "\\." "_" my_pm_ver "${CONFIG_PM_VERSION}" )
    string(TOUPPER  ${my_pm_ver} PluginVersionInfo_UC )
    list(APPEND CONFIG_FIND_LIBRARY_COMMAND_LIST
      "set( ${${product}_UC}_${PluginVersionInfo_UC} \"${mypmdir}/CetSkelPlugins/${product}/${CONFIG_PM_VERSION}\" )" )
    #message(STATUS "${${product}_UC}_${PluginVersionInfo_UC} ${mypmdir}/CetSkelPlugins/${product}/${CONFIG_PM_VERSION} " )
  endif()
  # add to pm list for package configure file
  foreach( my_pm ${CONFIG_PERL_PLUGIN_LIST} )
    #message( STATUS "config_pm: my_pm ${my_pm}")
    get_filename_component( my_pm_name ${my_pm} NAME )
    string(REGEX REPLACE "\\." "_" my_pm_dash "${my_pm_name}" )
    #message( STATUS "config_pm: my_pm_dash ${my_pm_dash}")
    string(TOUPPER  ${my_pm_dash} ${my_pm_name}_UC )
    list(APPEND CONFIG_FIND_LIBRARY_COMMAND_LIST
      "set( ${${my_pm_name}_UC} \"${mypmdir}${my_pm}\" )" )
    #message(STATUS "${${my_pm_name}_UC}  ${mypmdir}${my_pm} " )
  endforeach(my_pm)
  foreach( my_pm ${CONFIG_PM_LIST} )
    #message( STATUS "config_pm: my_pm ${my_pm}")
    get_filename_component( my_pm_name ${my_pm} NAME )
    string(REGEX REPLACE "\\." "_" my_pm_dash "${my_pm}" )
    #message( STATUS "config_pm: my_pm_dash ${my_pm_dash}")
    string(REGEX REPLACE "/" "_" my_pm_slash "${my_pm_dash}" )
    #message( STATUS "config_pm: my_pm_slash ${my_pm_slash}")
    string(TOUPPER  ${my_pm_slash} ${my_pm_name}_UC )
    list(APPEND CONFIG_FIND_LIBRARY_COMMAND_LIST
      "set( ${${product}_UC}${${my_pm_name}_UC} \"${mypmdir}${my_pm}\" )" )
    #message(STATUS "${${product}_UC}${${my_pm_name}_UC}  ${mypmdir}${my_pm} " )
  endforeach(my_pm)

  _config_package_config_file()

  # allowed COMPATIBILITY values are:
  # AnyNewerVersion ExactVersion SameMajorVersion
  if( CCC_NO_FLAVOR )
    cet_write_version_file(
               ${CMAKE_CURRENT_BINARY_DIR}/${product}ConfigVersion.cmake
	       VERSION ${cet_dot_version}
	       COMPATIBILITY AnyNewerVersion )
  else()
    write_basic_package_version_file(
               ${CMAKE_CURRENT_BINARY_DIR}/${product}ConfigVersion.cmake
	       VERSION ${cet_dot_version}
	       COMPATIBILITY AnyNewerVersion )
  endif()

  install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${product}Config.cmake
        	 ${CMAKE_CURRENT_BINARY_DIR}/${product}ConfigVersion.cmake
           DESTINATION ${distdir} )

endmacro( cet_cmake_config )
