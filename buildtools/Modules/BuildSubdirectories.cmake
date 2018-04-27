

macro( build_subdirectories )
  #message(STATUS "BUILD_SUBSIRECTORIES: called with ${ARGC} arguments: ${ARGV}")
  set(build_subdirectories_usage "USAGE: build_subdirectories( <package list> )")
  set(subdir_list ${ARGN})
  #message(STATUS "BUILD_SUBSIRECTORIES: CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} ")
  FILE(GLOB subdir_cmake_files */CMakeLists.txt  )
  #message(STATUS "BUILD_SUBSIRECTORIES: found ${subdir_cmake_files} ")

  foreach( cdir ${subdir_cmake_files} )
      get_filename_component(dir2 ${cdir} PATH)
      get_filename_component(dir ${dir2} NAME)
      message(STATUS "BUILD_SUBSIRECTORIES: ${cdir} -> ${dir2} -> ${dir}" )
      message(STATUS "BUILD_SUBSIRECTORIES: including ${dir} ")
      add_subdirectory( ${dir} )
  endforeach(cdir)
endmacro( build_subdirectories )
