# Create the project shell environment setup script

if(APPLE)
  set(DY "DY")
endif()

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/../templates/setup.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/setup-ariel.sh
  @ONLY
  )

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/setup-ariel.sh
  DESTINATION "${${product}_bin_dir}"
  )
