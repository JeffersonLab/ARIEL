#include "cetlib/BasicPluginFactory.h"
#include "cetlib/os_libpath.h"

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
  : PluginFactory{search_path{os_libpath()}, suffix}
  , makerName_{makerName}
  , pluginTypeFuncName_{pluginTypeFuncName}
{}
