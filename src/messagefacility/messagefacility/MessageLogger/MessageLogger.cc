#include "messagefacility/MessageLogger/MessageLogger.h"
// vim: set sw=2 expandtab :

#include "cetlib/BasicPluginFactory.h"
#include "cetlib/HorizontalRule.h"
#include "cetlib/container_algorithms.h"
#include "cetlib/exempt_ptr.h"
#include "cetlib/os_libpath.h"
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
#include "messagefacility/Utilities/bold_fontify.h"
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
//#include <mutex>
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

namespace mf {
  namespace service {

    namespace ELdestConfig {

      enum dest_config { ORDINARY, STATISTICS };

    } // namespace ELdestConfig

  } // namespace service
} // namespace mf

namespace mf {

  namespace {

    cet::search_path
    getPluginPath()
    {
      if (getenv("MF_PLUGIN_PATH") != nullptr) {
        return cet::search_path{"MF_PLUGIN_PATH"};
      }
      return cet::search_path{cet::os_libpath()};
    }

    cet::BasicPluginFactory pluginFactory_{getPluginPath(), "mfPlugin"};
    cet::BasicPluginFactory pluginStatsFactory_{getPluginPath(),
                                                "mfStatsPlugin"};
    atomic<bool> isStarted{false};
    map<string const, unique_ptr<service::ELdestination>> destinations_;
    bool cleanSlateConfiguration_{true};
    atomic<bool> purgeMode_{false};
    atomic<int> count_{0};
    // atomic<bool> messageBeingSent_{false};
    // atomic<bool> waitingMessagesBusy_{false};
    // mutex msgMutex_;
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
            if (!hostaddr_.empty() && hostaddr_.compare("127.0.0.1") &&
                hostaddr_.compare("::1")) {
              break;
            }
          }
          if (hostaddr_.empty()) {
            // Failed to find anything, use the loopback address.
            hostaddr_ = "127.0.0.1";
          }
        }
      }
      if (!applicationName.empty()) {
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

  } // unnamed namespace

  namespace {

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
      if (msg.xid().hostname().empty()) {
        msg.setHostName(GetHostName());
      }
      if (msg.xid().hostaddr().empty()) {
        msg.setHostAddr(GetHostAddr());
      }
      if (msg.xid().application().empty()) {
        msg.setApplication(GetApplicationName());
      }
      if (msg.xid().pid() == 0) {
        msg.setPID(GetPid());
      }
      if (destinations.empty()) {
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
                     service::ELdestConfig::dest_config const configuration)
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
        if (configuration == service::ELdestConfig::STATISTICS) {
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
          if (configuration == service::ELdestConfig::STATISTICS) {
            throw mf::Exception{mf::errors::Configuration}
              << "\n"
              << " Output identifier: \"" << outputId << "\""
              << " already specified within ordinary/statistics block in FHiCL "
                 "file"
              << "\n";
          }
        }
        auto iter_id_dest = destinations_.find(outputId);
        if (iter_id_dest != destinations_.end()) {
          string const hrule{"\n==============================================="
                             "============================= \n"};
          ostringstream except_msg;
          except_msg << hrule << "\n    Duplicate name for a ";
          if (configuration == service::ELdestConfig::ORDINARY) {
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
        auto& plugin_factory =
          (configuration == service::ELdestConfig::STATISTICS) ?
            pluginStatsFactory_ :
            pluginFactory_;
        try {
          destinations_[outputId] =
            makePlugin_(plugin_factory, libspec, psetname, dest_pset);
        }
        catch (fhicl::detail::validationException const& e) {
          string msg{"Configuration error for destination: " +
                     detail::bold_fontify(psetname) + "\n\n"};
          msg += e.what();
          config_errors.push_back(move(msg));
        }
      }
      if (!config_errors.empty()) {
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
      makeDestinations(ordinaryDests, service::ELdestConfig::ORDINARY);
      auto statDests = dest_psets.get<fhicl::ParameterSet>(
        "statistics", default_statistics_config);
      makeDestinations(statDests, service::ELdestConfig::STATISTICS);
    }

    void
    logMessage(ErrorObj* msg)
    {
      if (purgeMode_) {
        // We are dropping all messages due to an earlier exception.
        delete msg;
        return;
      }
      // bool expected = false;
      // while (!messageBeingSent_.compare_exchange_strong(expected, true)) {
      //  // Some other thread is already processing a message,
      //  // so if the queue is not being emptied right now, append
      //  // this message to the queue and return, otherwise, wait
      //  // for the empty cycle to finish, and retry.
      //  bool waiting_expected = false;
      //  if (waitingMessagesBusy_.compare_exchange_strong(waiting_expected,
      //  true)) {
      //    // The thread which is already processing messages has not
      //    // begun emptying the waiting message queue yet, we can add
      //    // our message to the queue and return.
      //    waitingMessages_.push(msg);
      //    waitingMessagesBusy_.store(false);
      //    return;
      //  }
      //  waiting_expected = false;
      //  // The thread which is already processing messages has begun
      //  // to empty the waiting message queue, let him do that by spin
      //  // waiting, with a hardware pause of about 40 cycles between spins.
      //  while (!waitingMessagesBusy_.compare_exchange_strong(waiting_expected,
      //  true)) {
      //    asm volatile("pause\n" : : : "memory");
      //    waiting_expected = false;
      //  }
      //  waitingMessagesBusy_.store(false);
      //  // Other guy has finished emptying the queue and released
      //  // messageBeingSent_ too. We may now compete with other threads
      //  // to set messageBeingSent_. If we fail, we will try to queue
      //  // the message again.
      //  expected = false;
      //}
      // lock_guard<mutex> blocker(msgMutex_);
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
      //// process any waiting messages
      // unique_ptr<ErrorObj> waiting_msgHolder;
      // ErrorObj* waiting_msg = nullptr;
      //// Force any other threads sending messages which want to add
      //// their message to the queue to spin wait while we empty it.
      ////waitingMessagesBusy_.store(true);
      // bool waiting_expected = false;
      //// The thread which is already processing messages has begun
      //// to empty the waiting message queue, let him do that by spin
      //// waiting, with a hardware pause of about 40 cycles between spins.
      // while (!waitingMessagesBusy_.compare_exchange_strong(waiting_expected,
      // true)) {
      //  asm volatile("pause\n" : : : "memory");
      //  waiting_expected = false;
      //}
      // while (waitingMessages_.try_pop(waiting_msg)) {
      //  if (purgeMode_) {
      //    delete waiting_msg;
      //    waiting_msg = nullptr;
      //    continue;
      //  }
      //  try {
      //    waiting_msgHolder.reset(waiting_msg);
      //    waiting_msg->setReactedTo(false);
      //    sendMsgToDests(*waiting_msg, destinations_);
      //  }
      //  catch (cet::exception const& e) {
      //    ++count_;
      //    cerr << "MessageLoggerScribe caught " << count_ << "
      //    cet::exceptions, text = \n" << e.what() << "\n"; if (count_ > 25) {
      //      cerr << "MessageLogger will no longer be processing messages due
      //      to errors (entering purge mode).\n"; purgeMode_ = true;
      //    }
      //  }
      //  catch (...) {
      //    cerr << "MessageLoggerScribe caught an unknown exception and will no
      //    longer be processing messages. (entering purge mode)\n"; purgeMode_
      //    = true;
      //  }
      //}
      //// At this point we have sent our message and emptied the
      //// queue. Now allow other threads to send messages.
      // messageBeingSent_.store(false);
      //// And also allow those threads that are spinning waiting
      //// for the queue to empty to proceed to compete with the
      //// other threads to send or queue their message.
      //// Note: If we did this before allowing other senders then
      //// the threads waiting for the queue to empty, might simply
      //// queue their messages and if there are no other senders
      //// for a while their messages might pend for a long time.
      // waitingMessagesBusy_.store(false);
    }

    void
    summarize()
    {
      try {
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
                 detail::bold_fontify("cerr_early") + "\n\n"};
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
    // If there are any waiting messages, finish them off.
    // FIXME: The code has been changed to make this case impossible, remove!
    // FIXME: We need to disable sending any more while we are tying to empty
    // the queue!
    // ErrorObj* msg = nullptr;
    // while (waitingMessages_.try_pop(msg)) {
    //  if (!purgeMode_) {
    //    msg->setReactedTo(false);
    //    sendMsgToDests(*msg, destinations_);
    //  }
    //  delete msg;
    //}
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
  SetPid(long pid)
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
