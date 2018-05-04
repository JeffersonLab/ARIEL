#include "messagefacility/plugins/stringstream.h"
// vim: set sw=2 expandtab :

#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ProvideMakePluginMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/Utilities/exception.h"

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

using namespace std;
using mf::service::ELostreamOutput;

namespace {

  map<string, cet::ostream_handle> streams;

} // unnamed namespace

ostringstream&
mf::getStringStream(string const& psetName)
{
  auto const I = streams.find(psetName);
  if (I != streams.end()) {
    return dynamic_cast<ostringstream&>(static_cast<ostream&>(I->second));
  }
  throw Exception(errors::Configuration, "mf::getStringStream:")
    << "Unable to find specified messagefacility logging stream \"" << psetName
    << ":\" please ensure it has been configured as a destination.";
}

MAKE_PLUGIN_START(auto, string const& psetName, fhicl::ParameterSet const& pset)
{
  auto ret = streams.emplace(psetName, cet::ostream_handle{ostringstream{}});
  return make_unique<ELostreamOutput>(pset, ret.first->second);
}
MAKE_PLUGIN_END

CET_PROVIDE_FILE_PATH()
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)
FHICL_PROVIDE_ALLOWED_CONFIGURATION(ELostreamOutput)
