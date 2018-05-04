// vim: set sw=2 expandtab :

#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ProvideMakePluginMacros.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <string>

using namespace std;

namespace {

  class Issue17457TestDestination : public mf::service::ELostreamOutput {

  public:
    struct Config {
      fhicl::TableFragment<mf::service::ELostreamOutput::Config>
        elOstreamConfig;
      fhicl::OptionalAtom<string> application{fhicl::Name{"application"}};
      fhicl::OptionalAtom<string> hostName{fhicl::Name{"hostName"}};
      fhicl::OptionalAtom<string> hostAddr{fhicl::Name{"hostAddr"}};
      fhicl::OptionalAtom<long> pid{fhicl::Name{"pid"}};
    };

    using Parameters = fhicl::WrappedTable<Config>;

  public:
    Issue17457TestDestination(Parameters const&);

  private:
    void fillPrefix(ostringstream& os, mf::ErrorObj const& msg) override;
  };

  Issue17457TestDestination::Issue17457TestDestination(Parameters const& ps)
    : mf::service::ELostreamOutput{ps().elOstreamConfig(),
                                   cet::ostream_handle{cerr}}
  {
    string tmp;
    if (ps().application(tmp)) {
      mf::SetApplicationName(tmp);
    }
    if (ps().hostName(tmp)) {
      mf::SetHostName(tmp);
    }
    if (ps().hostAddr(tmp)) {
      mf::SetHostAddr(tmp);
    }
    long pid{};
    if (ps().pid(pid)) {
      mf::SetPid(pid);
    }
  }

  void
  Issue17457TestDestination::fillPrefix(ostringstream& oss,
                                        mf::ErrorObj const& msg)
  {
    ELostreamOutput::fillPrefix(oss, msg);
    auto const& xid = msg.xid();
    oss << "\nApplication = " << xid.application()
        << ", hostname = " << xid.hostname()
        << ", hostaddr = " << xid.hostaddr() << ", pid = " << xid.pid()
        << ".\n";
  }

} // unnamed namespace

MAKE_PLUGIN_START(auto, string const&, fhicl::ParameterSet const& pset)
{
  return make_unique<Issue17457TestDestination>(pset);
}
MAKE_PLUGIN_END

CET_PROVIDE_FILE_PATH()
FHICL_PROVIDE_ALLOWED_CONFIGURATION(Issue17457TestDestination)
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)
