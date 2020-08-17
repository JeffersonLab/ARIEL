// vim: set sw=2 expandtab :

#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ProvideMakePluginMacros.h"
#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/Ntuple.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

using namespace std;
using namespace std::string_literals;

namespace {

  using namespace cet::sqlite;

  ConnectionFactory factory;

  class sqlite3Plugin : public mf::service::ELdestination {

  public:
    struct Config {

      fhicl::TableFragment<ELdestination::Config> elDestConfig;
      fhicl::Atom<string> filename{fhicl::Name{"filename"}};
    };

    using Parameters = fhicl::WrappedTable<Config>;

  public:
    ~sqlite3Plugin();
    sqlite3Plugin(Parameters const&);

  private:
    void
    fillPrefix(ostringstream&, mf::ErrorObj const&) override
    {}
    void
    fillSuffix(ostringstream&, mf::ErrorObj const&) override
    {}
    void routePayload(ostringstream const&, mf::ErrorObj const&) override;

  private:
    Connection* connection_;
    Ntuple<string,
           string,
           string,
           string,
           string,
           string,
           unsigned,
           string,
           string,
           string>* msgTable_;
  };

  sqlite3Plugin::~sqlite3Plugin()
  {
    delete msgTable_;
    msgTable_ = nullptr;
    delete connection_;
    connection_ = nullptr;
  }

  sqlite3Plugin::sqlite3Plugin(Parameters const& ps)
    : ELdestination{ps().elDestConfig()}
    , connection_{factory.make_connection(ps().filename())}
    , msgTable_{new Ntuple<string,
                           string,
                           string,
                           string,
                           string,
                           string,
                           unsigned,
                           string,
                           string,
                           string>{*connection_,
                                   "Messages"s,
                                   {{"Timestamp"s,
                                     "Hostname"s,
                                     "Hostaddress"s,
                                     "Severity"s,
                                     "Category"s,
                                     "AppName"s,
                                     "ProcessId"s,
                                     "RunEventNo"s,
                                     "ModuleName"s,
                                     "Message"s}}}}
  {}

  void
  sqlite3Plugin::routePayload(ostringstream const& oss, mf::ErrorObj const& msg)
  {
    auto const& xid = msg.xid();
    string const& timestamp = format_.timestamp(msg.timestamp());
    string const& hostname = xid.hostname();
    string const& hostaddr = xid.hostaddr();
    string const& severity = xid.severity().getName();
    string const& category = xid.id();
    string const& app = xid.application();
    long const& pid = xid.pid();
    string const& iterationNo = mf::GetIteration();
    string const& modname = xid.module();
    string const& usrMsg =
      !oss.str().compare(0, 1, "\n") ? oss.str().erase(0, 1) : oss.str();
    msgTable_->insert(timestamp,
                      hostname,
                      hostaddr,
                      severity,
                      category,
                      app,
                      pid,
                      iterationNo,
                      modname,
                      usrMsg);
  }

} // unnamed namespace

MAKE_PLUGIN_START(auto, string const&, fhicl::ParameterSet const& pset)
{
  return make_unique<sqlite3Plugin>(pset);
}
MAKE_PLUGIN_END

CET_PROVIDE_FILE_PATH()
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)
FHICL_PROVIDE_ALLOWED_CONFIGURATION(sqlite3Plugin)
