#include "messagefacility/MessageLogger/MessageLogger.h"
// vim: set sw=2 expandtab :

#include "cetlib/BasicPluginFactory.h"
#include "cetlib/HorizontalRule.h"
#include "cetlib/bold_fontify.h"
#include "cetlib/container_algorithms.h"
#include "cetlib/exempt_ptr.h"
#include "cetlib/plugin_libpath.h"
#include "cetlib/propagate_const.h"
#include "cetlib/trim.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/types/OptionalDelegatedParameter.h"
#include "fhiclcpp/types/detail/validationException.h"
#include "hep_concurrency/RecursiveMutex.h"
#include "messagefacility/MessageLogger/MFConfig.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/Utilities/exception.h"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <map>
#include <memory>

#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>

using namespace std;
using namespace std::string_literals;
using namespace hep::concurrency;

namespace {
  enum class destination_kind { ordinary, statistics };
}

namespace mf {

  namespace {

    cet::search_path
    getPluginPath()
    {
      if (getenv("MF_PLUGIN_PATH") != nullptr) {
        return cet::search_path{"MF_PLUGIN_PATH"};
      }
      return cet::search_path{cet::plugin_libpath(), std::nothrow};
    }

    cet::BasicPluginFactory pluginFactory_{getPluginPath(), "mfPlugin"};
    cet::BasicPluginFactory pluginStatsFactory_{getPluginPath(),
                                                "mfStatsPlugin"};
    atomic<bool> isStarted{false};
    map<string const, unique_ptr<service::ELdestination>> destinations_;
    bool cleanSlateConfiguration_{true};
    atomic<bool> purgeMode_{false};
    atomic<int> count_{0};
    RecursiveMutex msgMutex_{"MessageLogger::msgMutex_"};
    string hostname_;
    string hostaddr_;
    string application_;
    long pid_{};

    // Any text you want.  It goes into the message header in the module
    // position.
    thread_local string module_ = "Early";

    // Kind of pointless, arbitrarily set to an alternate spelling of
    // the phase, or the {run|subrun|event} number by art::MFStatusUpdater.
    // FIXME: The statistics gatherer attempts to parse this!
    thread_local string iteration_ = "pre-events";

    int
    initGlobalVars(string const& applicationName = "")
    {
      char hostname[1024] = {0};
      hostname_ =
        (gethostname(hostname, 1023) == 0) ? hostname : "Unknown Host";
      hostent* host = nullptr;
      host = gethostbyname(hostname);
      if (host != nullptr) {
        // Host lookup succeeded.
        char* ip = inet_ntoa(*(struct in_addr*)host->h_addr);
        hostaddr_ = ip;
      } else {
        // Loop over all network interfaces and use the first non-loopback
        // address found.
        ifaddrs* ifAddrStruct = nullptr;
        if (getifaddrs(&ifAddrStruct)) {
          // Failed, use the loopback address.
          hostaddr_ = "127.0.0.1";
        } else {
          // Loop over all interfaces.
          for (ifaddrs* ifa = ifAddrStruct; ifa != nullptr;
               ifa = ifa->ifa_next) {
            if (ifa->ifa_addr->sa_family == AF_INET) {
              // This interface has a valid IPv4 address.
              void* tmpAddrPtr = &((sockaddr_in*)ifa->ifa_addr)->sin_addr;
              char addressBuffer[INET_ADDRSTRLEN];
              inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
              hostaddr_ = addressBuffer;
            } else if (ifa->ifa_addr->sa_family == AF_INET6) {
              // This interface has a valid IPv6 address.
              void* tmpAddrPtr = &((sockaddr_in6*)ifa->ifa_addr)->sin6_addr;
              char addressBuffer[INET6_ADDRSTRLEN];
              inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
              hostaddr_ = addressBuffer;
            }
            // Use the address if it is not a loopback address.
            if (!empty(hostaddr_) && hostaddr_.compare("127.0.0.1") &&
                hostaddr_.compare("::1")) {
              break;
            }
          }
          if (empty(hostaddr_)) {
            // Failed to find anything, use the loopback address.
            hostaddr_ = "127.0.0.1";
          }
        }
      }
      if (!empty(applicationName)) {
        application_ = applicationName;
      } else {
        // get process name from '/proc/pid/cmdline'
        stringstream ss;
        ss << "//proc//" << pid_ << "//cmdline";
        ifstream procfile{ss.str().c_str()};
        string procinfo;
        if (procfile.is_open()) {
          // FIXME: This can fail with ERETRY!
          procfile >> procinfo;
          procfile.close();
        }
        auto end = procinfo.find('\0');
        auto start = procinfo.find_last_of('/', end);
        application_ = procinfo.substr(start + 1, end - start - 1);
      }
      pid_ = static_cast<long>(getpid());
      return 0;
    }

    // FIXME: Do not want to read from /proc/pid/cmdline if we do not have to!
    // static int globalsInitializer = initGlobalVars();

    unique_ptr<service::ELdestination>
    makePlugin_(cet::BasicPluginFactory& plugin_factory,
                string const& libspec,
                string const& psetname,
                fhicl::ParameterSet const& pset)
    {
      unique_ptr<service::ELdestination> result;
      try {
        auto const pluginType = plugin_factory.pluginType(libspec);
        if (pluginType ==
            cet::PluginTypeDeducer<service::ELdestination>::value) {
          result =
            plugin_factory.makePlugin<unique_ptr<service::ELdestination>>(
              libspec, psetname, pset);
        } else {
          throw Exception(errors::Configuration, "MessageLoggerScribe: ")
            << "unrecognized plugin type " << pluginType << "for plugin "
            << libspec << ".\n";
        }
      }
      catch (cet::exception const& e) {
        throw Exception(errors::Configuration, "MessageLoggerScribe: ", e)
          << "Exception caught while processing plugin spec.\n";
      }
      return result;
    }

    string const default_destination_config_string = "  type: cerr"
                                                     "  categories: {"
                                                     "    default: {"
                                                     "      limit: -1"
                                                     "    }"
                                                     "  }"s;
    fhicl::ParameterSet
    default_destination_config()
    {
      fhicl::ParameterSet result;
      fhicl::make_ParameterSet(default_destination_config_string, result);
      return result;
    }

    fhicl::ParameterSet
    default_destination_set_config()
    {
      string const config{"cerr: { "s + default_destination_config_string +
                          " }"s};
      fhicl::ParameterSet result;
      fhicl::make_ParameterSet(config, result);
      return result;
    }

    void
    sendMsgToDests(
      ErrorObj& msg,
      map<string const, unique_ptr<service::ELdestination>>& destinations)
    {
      if (empty(msg.xid().hostname())) {
        msg.setHostName(GetHostName());
      }
      if (empty(msg.xid().hostaddr())) {
        msg.setHostAddr(GetHostAddr());
      }
      if (empty(msg.xid().application())) {
        msg.setApplication(GetApplicationName());
      }
      if (msg.xid().pid() == 0) {
        msg.setPID(GetPid());
      }
      if (empty(destinations)) {
        cerr << "\nERROR LOGGED WITHOUT DESTINATION!\nAttaching destination "
                "\"cerr\" by default\n\n";
        destinations.emplace(
          "cerr",
          make_unique<service::ELostreamOutput>(
            default_destination_set_config(), cet::ostream_handle{cerr}));
      }
      for (auto& destid_and_destination : destinations) {
        destid_and_destination.second->log(msg);
      }
    }

    void
    makeDestinations(fhicl::ParameterSet const& dests,
                     destination_kind const configuration)
    {
      set<string> ids;
      vector<string> config_errors;
      for (auto const& psetname : dests.get_pset_names()) {
        auto dest_pset = dests.get<fhicl::ParameterSet>(psetname);
        if (dest_pset.is_empty()) {
          dest_pset = default_destination_config();
        }
        // Grab the destination type and filename.
        string dest_type{};
        if (!dest_pset.get_if_present("type", dest_type)) {
          throw Exception(errors::Configuration)
            << "No 'type' specified for destination '" << psetname << "'.\n";
        }
        if (configuration == destination_kind::statistics) {
          if ((dest_type != "cout"s) && (dest_type != "cerr"s) &&
              (dest_type != "file"s)) {
            throw mf::Exception{mf::errors::Configuration}
              << "\n"
              << "Unsupported type [ " << dest_type
              << " ] chosen for statistics printout.\n"
              << "Must choose ostream type: \"cout\", \"cerr\", or \"file\""
              << "\n";
          }
        }
        string outputId{dest_type};
        if ((dest_type != "cout"s) && (dest_type != "cerr"s) &&
            (dest_type != "syslog"s)) {
          outputId += ":" + dest_pset.get<string>("filename", psetname);
        }
        if (!ids.emplace(outputId).second) {
          // We have a duplicate.
          if (configuration == destination_kind::statistics) {
            throw mf::Exception{mf::errors::Configuration}
              << " Output identifier: \"" << outputId << "\""
              << " already specified within ordinary/statistics block in FHiCL "
                 "file"
              << "\n";
          }
        }
        auto iter_id_dest = destinations_.find(outputId);
        if (iter_id_dest != destinations_.end()) {
          constexpr cet::HorizontalRule rule{76};
          string const hrule{'\n' + rule('=') + '\n'};
          ostringstream except_msg;
          except_msg << hrule << "\n    Duplicate name for a ";
          if (configuration == destination_kind::ordinary) {
            except_msg << "MessageLogger";
          } else {
            except_msg << "MessageLogger Statistics";
          }
          except_msg << " destination: \"" << outputId << '"';
          ostringstream orig_config_msg;
          orig_config_msg
            << "\n    Only original configuration instructions are used. \n"
            << hrule;
          if (cleanSlateConfiguration_) {
            LogError("duplicateDestination")
              << except_msg.str() << orig_config_msg.str();
          } else {
            LogWarning("duplicateDestination")
              << except_msg.str() << orig_config_msg.str();
          }
          continue;
        }
        string const& libspec = dest_type;
        auto& plugin_factory = (configuration == destination_kind::statistics) ?
                                 pluginStatsFactory_ :
                                 pluginFactory_;
        try {
          destinations_[outputId] =
            makePlugin_(plugin_factory, libspec, psetname, dest_pset);
        }
        catch (fhicl::detail::validationException const& e) {
          string msg{"Configuration error for destination: " +
                     cet::bold_fontify(psetname) + "\n\n"};
          msg += e.what();
          config_errors.push_back(move(msg));
        }
      }
      if (!empty(config_errors)) {
        string msg{"\nThe following messagefacility destinations have "
                   "configuration errors:\n\n"};
        constexpr cet::HorizontalRule rule{60};
        msg += rule('=');
        msg += "\n\n";
        auto start = cbegin(config_errors);
        msg += *start;
        ++start;
        for (auto it = start, e = cend(config_errors); it != e; ++it) {
          msg += rule('-');
          msg += "\n\n";
          msg += *it;
        }
        msg += rule('=');
        msg += "\n\n";
        throw Exception(errors::Configuration) << msg;
      }
    }

    void
    configure(MFDestinationConfig::Config const& config)
    {
      if (destinations_.size() > 1) {
        LogWarning("multiLogConfig")
          << "The message logger has been configured multiple times";
        cleanSlateConfiguration_ = false;
      }
      fhicl::ParameterSet dest_psets;
      fhicl::ParameterSet ordinaryDests;
      if (!config.destinations.get_if_present(dest_psets)) {
        dest_psets = default_destination_set_config();
      }
      ordinaryDests = dest_psets;
      ordinaryDests.erase("statistics");
      // Dial down the early destination once the ordinary destinations are
      // filled.
      destinations_["cerr_early"s]->setThreshold(ELhighestSeverity);
      fhicl::ParameterSet default_statistics_config;
      if (ordinaryDests.is_empty()) {
        string const default_config{"file_stats: {\n"
                                    "  type: file\n"
                                    "  filename: \"err.log\"\n"
                                    "  threshold: WARNING\n"
                                    "}\n"};
        fhicl::make_ParameterSet(default_config, default_statistics_config);
      }
      makeDestinations(ordinaryDests, destination_kind::ordinary);
      auto statDests = dest_psets.get<fhicl::ParameterSet>(
        "statistics", default_statistics_config);
      makeDestinations(statDests, destination_kind::statistics);
    }

    void
    logMessage(ErrorObj* msg)
    {
      if (purgeMode_) {
        // We are dropping all messages due to an earlier exception.
        delete msg;
        return;
      }
      RecursiveMutexSentry sentry{msgMutex_, __func__};
      // Ok, no other thread active, process the current message.
      try {
        unique_ptr<ErrorObj> msgHolder{msg};
        msg->setReactedTo(false);
        sendMsgToDests(*msg, destinations_);
      }
      catch (cet::exception const& e) {
        ++count_;
        cerr << "MessageLoggerScribe caught " << count_
             << " cet::exceptions, text = \n"
             << e.what() << "\n";
        if (count_ > 25) {
          cerr << "MessageLogger will no longer be processing messages due to "
                  "errors (entering purge mode).\n";
          purgeMode_ = true;
        }
      }
      catch (...) {
        cerr << "MessageLoggerScribe caught an unknown exception and will no "
                "longer be processing messages. (entering purge mode)\n";
        purgeMode_ = true;
      }
    }

    void
    summarize() try {
      for (auto& destid_and_dest : destinations_) {
        auto& dest = *destid_and_dest.second;
        dest.summary();
      }
    }
    catch (cet::exception const& e) {
      cerr << "MessageLoggerScribe caught exception during summarize:\n"
           << e.what() << "\n";
    }
    catch (...) {
      cerr << "MessageLoggerScribe caught unknown exception type during "
              "summarize. (Ignored)\n";
    }

  } // unnamed namespace

  // Note: Safe to call from multiple threads.
  bool
  isMessageProcessingSetUp()
  {
    return isStarted.load();
  }

  // Note: Safe to call from multiple threads.
  void
  LogErrorObj(ErrorObj* msg)
  {
    if (isStarted.load()) {
      logMessage(msg);
      return;
    }
    if (msg->is_verbatim()) {
      ostringstream buf;
      buf << msg->fullText() << '\n';
      cerr << buf.str();
    } else {
      ostringstream buf;
      buf << "%MSG" << msg->xid().severity().getSymbol() << ' '
          << msg->xid().id() << msg->idOverflow() << ": \n"
          << msg->fullText() << "\n"
          << "%MSG\n";
      cerr << buf.str();
    }
    delete msg;
  }

  // Note: Call this from single-threaded mode only!
  void
  LogStatistics()
  {
    if (isStarted.load()) {
      summarize();
    }
  }

  // Note: Obsolete!  Remove when user code migrated.
  void
  FlushMessageLog()
  {}

  // Note: Call this from single-threaded mode only!
  void
  StartMessageFacility(fhicl::ParameterSet const& pset,
                       string const& applicationName)
  {
    if (isStarted.load()) {
      return;
    }
    // FIXME: We should not have to call StartMessageFacility() to get these
    // initialized!
    initGlobalVars(applicationName);
    try {
      destinations_["cerr_early"s] = makePlugin_(
        pluginFactory_, "cerr", "cerr_early", default_destination_config());
    }
    catch (fhicl::detail::validationException const& e) {
      string msg{"\nConfiguration error for destination: " +
                 cet::bold_fontify("cerr_early") + "\n\n"};
      msg += e.what();
      throw Exception(errors::Configuration) << msg;
    }
    try {
      // Note: We make all the destinations here.
      configure(MFDestinationConfig::Config{
        MFConfig::Parameters{pset}().destinations()});
    }
    catch (Exception const& ex) {
      // FIXME: Hardly seems necessary to rethrow just to change the message to
      // say it was during configure!
      throw Exception(errors::OtherError,
                      "Exception from MessageLoggerScribe::configure",
                      ex);
    }
    isStarted.store(true);
  }

  // Note: Call this from single-threaded mode only!
  void
  EndMessageFacility()
  {
    isStarted.store(false);
    // FIXME: The finish() call in all known uses does nothing, the destination
    // dtor can probably handle this, remove!
    // map<string, cet::propagate_const<unique_ptr<service::ELdestination>>>
    for (auto& category_and_destination : destinations_) {
      category_and_destination.second->finish();
    }
  }

  // Note: Call this from single-threaded mode only!
  void
  SetApplicationName(string const& applicationName)
  {
    application_ = applicationName;
  }

  // Note: Call this from single-threaded mode only!
  void
  SetHostName(string const& hostname)
  {
    hostname_ = hostname;
  }

  // Note: Call this from single-threaded mode only!
  void
  SetHostAddr(string const& hostaddr)
  {
    hostaddr_ = hostaddr;
  }

  // Note: Call this from single-threaded mode only!
  void
  SetPid(long const pid)
  {
    pid_ = pid;
  }

  // Note: Call this from single-threaded mode only!
  string const&
  GetApplicationName()
  {
    return application_;
  }

  // Note: Call this from single-threaded mode only!
  string const&
  GetHostName()
  {
    return hostname_;
  }

  // Note: Call this from single-threaded mode only!
  string const&
  GetHostAddr()
  {
    return hostaddr_;
  }

  // Note: Call this from single-threaded mode only!
  long
  GetPid()
  {
    return pid_;
  }

  // Phase or {run|subrun|event} number.
  // Note: Obsolete!  Remove when user code migrated.
  // Note: This is thread specific and thread safe.
  void
  SetContextIteration(string const& val)
  {
    iteration_ = val;
  }

  // Phase or {run|subrun|event} number.
  // Note: This is thread specific and thread safe.
  void
  SetIteration(string const& val)
  {
    iteration_ = val;
  }

  string const&
  GetIteration()
  {
    return iteration_;
  }

  // Note: Obsolete!  Remove when user code migrated.
  // Note: This is thread specific and thread safe.
  void
  SetContextSinglet(string const& val)
  {
    module_ = val;
  }

  // Note: This is thread specific and thread safe.
  void
  SetModuleName(string const& val)
  {
    module_ = val;
  }

  string const&
  GetModuleName()
  {
    return module_;
  }

  // Note: Call this from single-threaded mode only!
  // FIXME: This does not give the same answer as before because it is no longer
  // per-module!
  // Note: Obsolete! Remove when user code migrated.
  bool
  isDebugEnabled()
  {
    return false;
  }

  // Note: Call this from single-threaded mode only!
  // FIXME: This does not give the same answer as before because it is no longer
  // per-module!
  // Note: Obsolete! Remove when user code migrated.
  bool
  isInfoEnabled()
  {
    return true;
  }

  // Note: Call this from single-threaded mode only!
  // FIXME: This does not give the same answer as before because it is no longer
  // per-module!
  // Note: Obsolete! Remove when user code migrated.
  bool
  isWarningEnabled()
  {
    return true;
  }

  // This static global will be destroyed before any other
  // file-local or global variables when this dll is unloaded
  // (because it is the last in the file), and will take care
  // of cleanup if the user forgot to call EndMessageFacility().
  static struct FinalShutdown {
    ~FinalShutdown()
    {
      if (isStarted.load()) {
        EndMessageFacility();
      }
    }
  } ensureShutdown;

} // namespace mf
