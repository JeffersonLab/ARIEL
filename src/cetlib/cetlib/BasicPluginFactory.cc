#include "cetlib/BasicPluginFactory.h"
#include "cetlib/plugin_libpath.h"

cet::BasicPluginFactory::BasicPluginFactory(
  cet::search_path const& search_path,
  std::string const& suffix,
  std::string const& makerName,
  std::string const& pluginTypeFuncName)
  : PluginFactory{search_path, suffix}
  , makerName_{makerName}
  , pluginTypeFuncName_{pluginTypeFuncName}
{}

cet::BasicPluginFactory::BasicPluginFactory(
  std::string const& suffix,
  std::string const& makerName,
  std::string const& pluginTypeFuncName)
  : BasicPluginFactory(search_path{plugin_libpath(), std::nothrow},
                       suffix,
                       makerName,
                       pluginTypeFuncName)
{}
