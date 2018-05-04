// vim: set sw=2 expandtab :

#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ProvideMakePluginMacros.h"
#include "cetlib/ostream_handle.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/plugins/FileConfig.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

using namespace std;
using namespace mf::service;

namespace mf {

  struct file_mfPluginConfig {

    struct Config {

      fhicl::TableFragment<ELostreamOutput::Config> ostream_dest;
      fhicl::TableFragment<mfplugins::FileConfig> file_config;
    };

    using Parameters = fhicl::WrappedTable<Config>;
  };

} // namespace mf

namespace {

  auto
  makePlugin_(mf::file_mfPluginConfig::Parameters const& ps)
  {
    cet::ostream_handle osh{ps().file_config().filename(),
                            ps().file_config().append() ? ios::app :
                                                          ios::trunc};
    return make_unique<ELostreamOutput>(ps().ostream_dest(), move(osh));
  }

} // unnamed namespace

MAKE_PLUGIN_START(auto, string const&, fhicl::ParameterSet const& pset)
{
  return makePlugin_(pset);
}
MAKE_PLUGIN_END

CET_PROVIDE_FILE_PATH()
DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)
FHICL_PROVIDE_ALLOWED_CONFIGURATION(mf::file_mfPluginConfig)
