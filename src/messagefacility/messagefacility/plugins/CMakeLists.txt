include(BasicPlugin)

set(mf_plugin_libraries
  MF_MessageLogger
  fhiclcpp
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY})

# ordinary destination plugins
basic_plugin(cout mfPlugin ${mf_plugin_libraries})
basic_plugin(cerr mfPlugin ${mf_plugin_libraries})
basic_plugin(file mfPlugin ${mf_plugin_libraries})
basic_plugin(syslog mfPlugin ${mf_plugin_libraries})
basic_plugin(sqlite mfPlugin ${mf_plugin_libraries} ${SQLITE3})
basic_plugin(stringstream mfPlugin ${mf_plugin_libraries})

# statistics destination plugins
basic_plugin(cout mfStatsPlugin ${mf_plugin_libraries})
basic_plugin(cerr mfStatsPlugin ${mf_plugin_libraries})
basic_plugin(file mfStatsPlugin ${mf_plugin_libraries})

install_headers()
install_source()
