# ======================================================================
#
# Build/install script
#
# ======================================================================

include(CetMake)

# ----------------------------------------------------------------------
# Build and install this project's library:

set(LIB_NAME ${PROJECT_NAME})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/shlib_utils.cc.in
  ${CMAKE_CURRENT_BINARY_DIR}/shlib_utils.cc @ONLY
  )

cet_make_library(LIBRARY_NAME ${LIB_NAME}
  SOURCE
  ${CMAKE_CURRENT_BINARY_DIR}/shlib_utils.cc
  MD5Digest.cc
  BasicPluginFactory.cc
  LibraryManager.cc
  sqlite/Connection.cc
  sqlite/Exception.cc
  sqlite/Transaction.cc
  sqlite/detail/DefaultDatabaseOpenPolicy.cc
  sqlite/detail/bind_parameters.cc
  sqlite/detail/column_constraint.cc
  sqlite/detail/normalize_statement.cc
  sqlite/exec.cc
  sqlite/helpers.cc
  sqlite/query_result.cc
  sqlite/select.cc
  sqlite/statistics.cc
  PluginFactory.cc
  base_converter.cc
  canonical_number.cc
  canonical_string.cc
  column_width.cc
  cpu_timer.cc
  crc32.cc
  detail/plugin_search_path.cc
  detail/wrapLibraryManagerException.cc
  filepath_maker.cc
  filesystem.cc
  getenv.cc
  include.cc
  includer.cc
  loadable_libraries.cc
  lpad.cc
  nybbler.cc
  parsed_program_options.cc
  replace_all.cc
  rpad.cc
  search_path.cc
  sha1.cc
  simple_stats.cc
  split_path.cc
  split_by_regex.cc
  LIBRARIES
  cetlib_except
  hep_concurrency
  Boost::filesystem
  Boost::program_options
  Boost::regex
  ${CMAKE_DL_LIBS}
  ${SQLITE3}
  ${OPENSSL_LIBRARIES}
  )


# ----------------------------------------------------------------------
# Build and install this project's executables:

cet_make_exec(inc-expand LIBRARIES ${LIB_NAME})
cet_make_exec(get-accessible-paths LIBRARIES ${LIB_NAME})

# ----------------------------------------------------------------------
# Install this project's headers and sources:

install_headers(SUBDIRS detail sqlite sqlite/detail)
install_source(SUBDIRS detail sqlite sqlite/detail)

# ======================================================================
# Testing
add_subdirectory(test)
