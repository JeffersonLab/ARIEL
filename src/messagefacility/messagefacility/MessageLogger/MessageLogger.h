#ifndef messagefacility_MessageLogger_MessageLogger_h
#define messagefacility_MessageLogger_MessageLogger_h
// vim: set sw=2 expandtab :

#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

namespace fhicl {

  class ParameterSet;

} // namespace fhicl

namespace mf {

  // Start and stop the system.
  // The system is still usable if StartMessageFacility has not been called, but
  // it logs only to cerr and does not process any configuration options.
  void StartMessageFacility(fhicl::ParameterSet const&,
                            std::string const& applicationName = "");
  void EndMessageFacility();

  // Will return true if StartMessageFacility has been called, and
  // EndMessageFacility has not yet been called.
  bool isMessageProcessingSetUp();

  // Log a message.
  void LogErrorObj(ErrorObj*);

  // Log collected statistics to configured statistics destinations.
  void LogStatistics();

  // OBSOLETE. Does nothing.
  void FlushMessageLog();

  // These four set the system-wide values, but they can still be
  // overridden on a per-message basis.
  void SetApplicationName(std::string const&);
  void SetHostName(std::string const&);
  void SetHostAddr(std::string const&);
  void SetPid(long);

  // These four set the system-wide values, but they can still be
  // overridden on a per-message basis.
  std::string const& GetApplicationName();
  std::string const& GetHostName();
  std::string const& GetHostAddr();
  long GetPid();

  // What is printed in the message header as the iteration (either
  // run/subrun/event number or phase), thread-local. Note: Obsolete!  Remove
  // when user code migrated.
  void SetContextIteration(std::string const&);

  // What is printed in the message header as the iteration (either
  // run/subrun/event number or phase), thread-local.
  void SetIteration(std::string const&);

  // What is printed in the message header as the iteration (either
  // run/subrun/event number or phase), thread-local.
  std::string const& GetIteration();

  // What is printed in the message header as the module name, thread-local.
  // Note: Obsolete!  Remove when user code migrated.
  void SetContextSinglet(std::string const&);

  // What is printed in the message header as the module name, thread-local.
  void SetModuleName(std::string const&);

  // What is printed in the message header as the module name, thread-local.
  std::string const& GetModuleName();

  // Obsolete! Delete these when user code migrated.
  bool isDebugEnabled();
  bool isInfoEnabled();
  bool isWarningEnabled();

  class NeverLogger_ {
  public:

    // Dedicated function for char const* to avoid unnecessary
    // template instantiations of char const[].  Will take precedence
    // over the template version.
    decltype(auto) operator<<(char const*)
    {
      return std::forward<NeverLogger_>(*this);
    }

    template <class T>
    decltype(auto) operator<<(T const&)
    {
      return std::forward<NeverLogger_>(*this);
    }

    decltype(auto)
    operator<<(std::ostream& (*)(std::ostream&))
    {
      return std::forward<NeverLogger_>(*this);
    }
    decltype(auto)
    operator<<(std::ios_base& (*)(std::ios_base&))
    {
      return std::forward<NeverLogger_>(*this);
    }
  };

  template <ELseverityLevel::ELsev_ SEV, bool VERBATIM>
  class MaybeLogger_ {

  public:
    MaybeLogger_(MaybeLogger_ const&) = delete;
    MaybeLogger_& operator=(MaybeLogger_ const&) = delete;
    MaybeLogger_& operator=(MaybeLogger_&&) = delete;

    ~MaybeLogger_()
    {
      if (msg_ == nullptr) {
        return;
      }
      try {
        msg_->setModule(GetModuleName());
        msg_->setIteration(GetIteration());
        LogErrorObj(msg_.release());
      }
      catch (...) {
        // FIXME: We should never ignore errors!
      }
    }

    MaybeLogger_() : msg_{} {}

    MaybeLogger_(MaybeLogger_&& rhs) noexcept : msg_{std::move(rhs.msg_)} {}

    MaybeLogger_(std::string const& category,
                 std::string const& file = "",
                 int line_number = 0)
      : msg_{}
    {
      // Verbatim messages have the full file path, otherwise just the basename.
      std::string filename{file};
      if (!VERBATIM) {
        auto const lastSlash = file.find_last_of('/');
        if ((lastSlash != std::string::npos) &&
            (lastSlash != (file.size() - 1))) {
          filename = file.substr(lastSlash + 1, file.size() - lastSlash - 1);
        }
      }
      msg_ = std::move(std::make_unique<ErrorObj>(
        SEV, category, VERBATIM, filename, line_number));
    }

    // Dedicated function for char const* to avoid unnecessary
    // template instantiations of char const[].  Will take precedence
    // over the template version.
    decltype(auto) operator<<(char const* s)
    {
      if (msg_) {
        (*msg_) << s;
      }
      return std::forward<MaybeLogger_>(*this);
    }

    template <class T>
    decltype(auto)
    operator<<(T const& t)
    {
      if (msg_) {
        (*msg_) << t;
      }
      return std::forward<MaybeLogger_>(*this);
    }

    decltype(auto)
    operator<<(std::ostream& (*f)(std::ostream&))
    {
      if (msg_) {
        (*msg_) << f;
      }
      return std::forward<MaybeLogger_>(*this);
    }

    decltype(auto)
    operator<<(std::ios_base& (*f)(std::ios_base&))
    {
      if (msg_) {
        (*msg_) << f;
      }
      return std::forward<MaybeLogger_>(*this);
    }

  private:
    std::unique_ptr<ErrorObj> msg_{};
  };

  //
  // Usage: LogXXX("category") << stuff. See also LOG_XXX macros, below.
  //
  // Statements follow pattern:
  //    using LogXXX = MaybeLogger_<severity-level, verbatim>;
  //
  // Verbatim: "No-frills" formatting.
  //

  // Non-verbatim messages, standard decorations.
  using LogDebug = MaybeLogger_<ELseverityLevel::ELsev_success, false>;
  using LogInfo = MaybeLogger_<ELseverityLevel::ELsev_info, false>;
  using LogWarning = MaybeLogger_<ELseverityLevel::ELsev_warning, false>;
  using LogError = MaybeLogger_<ELseverityLevel::ELsev_error, false>;
  using LogSystem = MaybeLogger_<ELseverityLevel::ELsev_severe, false>;

  // Verbatim messages, no decorations at all.
  using LogTrace = MaybeLogger_<ELseverityLevel::ELsev_success, true>;
  using LogVerbatim = MaybeLogger_<ELseverityLevel::ELsev_info, true>;
  using LogPrint = MaybeLogger_<ELseverityLevel::ELsev_warning, true>;
  using LogProblem = MaybeLogger_<ELseverityLevel::ELsev_error, true>;
  using LogAbsolute = MaybeLogger_<ELseverityLevel::ELsev_severe, true>;

} // namespace mf

// Non-verbatim messages, standard decorations.
// Note: LOG_DEBUG is below.
#define LOG_INFO(category) mf::LogInfo(category, __FILE__, __LINE__)
#define LOG_WARNING(category) mf::LogWarning(category, __FILE__, __LINE__)
#define LOG_ERROR(category) mf::LogError(category, __FILE__, __LINE__)
#define LOG_SYSTEM(category) mf::LogSystem(category, __FILE__, __LINE__)

// Verbatim messages, no decorations at all.
// Note: LOG_TRACE is below.
#define LOG_VERBATIM(category) mf::LogVerbatim(category, __FILE__, __LINE__)
#define LOG_PRINT(category) mf::LogPrint(category, __FILE__, __LINE__)
#define LOG_PROBLEM(category) mf::LogProblem(category, __FILE__, __LINE__)
#define LOG_ABSOLUTE(category) mf::LogAbsolute(category, __FILE__, __LINE__)

#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG

// Suppress LOG_DEBUG/TRACE if NDEBUG or ML_NDEBUG are set,
// except see below for ML_DEBUG which takes precendence.
#if defined(NDEBUG) || defined(ML_NDEBUG)
#define EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif // NDEBUG || ML_NDEBUG

// If ML_DEBUG is defined, LOG_DEBUG/TRACE are active unconditionally,
// no matter what NDEBUG or ML_NDEBUG say.
#ifdef ML_DEBUG
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif // ML_DEBUG

#ifdef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG

#define LOG_DEBUG(id)                                                          \
  mf::NeverLogger_ {}
#define LOG_TRACE(id)                                                          \
  mf::NeverLogger_ {}

#else // EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG

#define LOG_DEBUG(id)                                                          \
  mf::LogDebug { id, __FILE__, __LINE__ }
#define LOG_TRACE(id)                                                          \
  mf::LogTrace { id, __FILE__, __LINE__ }

#endif // EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG

#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG

#endif /* messagefacility_MessageLogger_MessageLogger_h */

// Local Variables:
// mode: c++
// End:
