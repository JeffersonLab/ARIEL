// vim: set sw=2 expandtab :

#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ProvideMakePluginMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELstatistics.h"

#include <iostream>
#include <memory>
#include <string>

using namespace std;
using mf::service::ELstatistics;

MAKE_PLUGIN_START(auto, string const&, fhicl::ParameterSet const& pset)
{
  return make_unique<ELstatistics>(pset, cout);
}
MAKE_PLUGIN_END

CET_PROVIDE_FILE_PATH()
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)
FHICL_PROVIDE_ALLOWED_CONFIGURATION(ELstatistics)
