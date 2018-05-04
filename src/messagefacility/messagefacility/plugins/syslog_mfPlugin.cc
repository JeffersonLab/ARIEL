// vim: set sw=2 expandtab :

#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ProvideMakePluginMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/exception.h"

#include <iostream>
#include <memory>
#include <string>

#include <syslog.h>

using namespace std;
using namespace std::string_literals;

namespace mfplugins {

  using mf::ELseverityLevel;
  using mf::ErrorObj;
  using mf::service::ELdestination;

  class ELsyslog : public ELdestination {

  public:
    struct Config {
      fhicl::TableFragment<ELdestination::Config> elDestConfig;
    };

    using Parameters = fhicl::WrappedTable<Config>;

  public:
    ELsyslog(Parameters const& pset);

  private:
    void fillPrefix(ostringstream&, ErrorObj const&) override;
    void fillUsrMsg(ostringstream&, ErrorObj const&) override;
    void fillSuffix(ostringstream&, ErrorObj const&) override;
    void routePayload(ostringstream const&, ErrorObj const&) override;
    int syslogLevel(ELseverityLevel);
  };

  ELsyslog::ELsyslog(Parameters const& pset)
    : ELdestination{pset().elDestConfig()}
  {
    openlog("MF", 0, LOG_LOCAL0);
  }

  void
  ELsyslog::fillPrefix(ostringstream& oss, ErrorObj const& msg)
  {
    auto const& xid = msg.xid();
    oss << format_.timestamp(msg.timestamp()) << '|' << xid.hostname() << '|'
        << xid.hostaddr() << '|' << xid.severity().getName() << '|' << xid.id()
        << '|' << xid.application() << '|' << xid.pid() << '|'
        << mf::GetIteration() << '|' << xid.module() << '|';
  }

  void
  ELsyslog::fillUsrMsg(ostringstream& oss, ErrorObj const& msg)
  {
    ostringstream buf;
    ELdestination::fillUsrMsg(buf, msg);
    string const& usrMsg =
      !buf.str().compare(0, 1, "\n") ? buf.str().erase(0, 1) : buf.str();
    oss << usrMsg;
  }

  void
  ELsyslog::fillSuffix(ostringstream&, ErrorObj const&)
  {}

  void
  ELsyslog::routePayload(ostringstream const& oss, ErrorObj const& msg)
  {
    int const severity = syslogLevel(msg.xid().severity());
    syslog(severity, "%s", oss.str().data());
  }

  int
  ELsyslog::syslogLevel(ELseverityLevel const severity)
  {
    //  Following syslog levels not used:
    //     LOG_EMERG  ; //0
    //     LOG_ALERT  ; //1
    //     LOG_NOTICE ; //5
    switch (severity.getLevel()) { // Used by:
      case ELseverityLevel::ELsev_severe:
        return LOG_CRIT; //   LogAbsolute, LogSystem
      case ELseverityLevel::ELsev_error:
        return LOG_ERR; //   LogError, LogProblem
      case ELseverityLevel::ELsev_warning:
        return LOG_WARNING; //   LogPrint, LogWarning
      case ELseverityLevel::ELsev_info:
        return LOG_INFO; //   LogInfo, LogVerbatim
      case ELseverityLevel::ELsev_success:
        return LOG_DEBUG; //   LogDebug, LogTrace
      default:
        throw mf::Exception(mf::errors::LogicError)
          << "ELseverityLevel: " << severity
          << " not currently supported for syslog destination\n";
    }
    return -1;
  }

} // namespace mfplugins

MAKE_PLUGIN_START(auto, string const&, fhicl::ParameterSet const& pset)
{
  return make_unique<mfplugins::ELsyslog>(pset);
}
MAKE_PLUGIN_END

CET_PROVIDE_FILE_PATH()
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)
FHICL_PROVIDE_ALLOWED_CONFIGURATION(mfplugins::ELsyslog)
