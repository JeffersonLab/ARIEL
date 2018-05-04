install(FILES PluginVersionInfo.pm.in
  DESTINATION ${${product}_perllib})

cet_script(cetskelgen)

add_subdirectory(CetSkel)
