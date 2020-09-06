#ifndef messagefacility_MessageService_ELdestination_h
#define messagefacility_MessageService_ELdestination_h
// vim: set sw=2 expandtab :

#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/OptionalDelegatedParameter.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <bitset>
#include <cstddef>
#include <map>
#include <ostream>
#include <sstream>
#include <string>

namespace mf::service {

  class ELdestination {
  public:
    enum flag_enum {
      NO_LINE_BREAKS,
      TIMESTAMP,
      MILLISECOND,
      MODULE,
      SUBROUTINE,
      TEXT,
      SOME_CONTEXT,
      SERIAL,
      FULL_CONTEXT,
      TIME_SEPARATE,
      EPILOGUE_SEPARATE,
      NFLAGS
    };

    struct Category {
      struct Config {
        ~Config();
        Config();
        explicit Config(fhicl::ParameterSet const& pset);

        static std::string limit_comment();
        static std::string reportEvery_comment();
        static std::string timespan_comment();

        fhicl::Atom<int> limit{fhicl::Name{"limit"}, -1};
        fhicl::Atom<int> reportEvery{fhicl::Name{"reportEvery"}, -1};
        fhicl::Atom<int> timespan{fhicl::Name{"timespan"}, -1};
      };

      ~Category();
      Category();
    };

    struct CategoryParams {
      ~CategoryParams();
      CategoryParams();

      int limit_;
      int reportEvery_;
      int timespan_;
    };

    struct XidLimiter {
      ~XidLimiter();
      XidLimiter();

      time_t previousTimestamp_;
      int msgCount_{};
      int skippedMsgCount_;

      int limit_;
      int reportEvery_;
      int timespan_;
    };

    struct StatsCount {
      ~StatsCount();
      StatsCount();
      void add(std::string const& context, bool reactedTo);

      int n_{};
      int aggregateN_{};
      bool ignoredFlag_{false};
      std::string context1_{};
      std::string context2_{};
      std::string contextLast_{};
    };

    class MsgFormatSettings {
    public:
      struct Config {
        ~Config();
        Config();

        fhicl::Atom<std::string> timestamp;
        fhicl::Atom<bool> noLineBreaks;
        fhicl::Atom<unsigned long long> lineLength;
        fhicl::Atom<bool> wantModule;
        fhicl::Atom<bool> wantSubroutine;
        fhicl::Atom<bool> wantText;
        fhicl::Atom<bool> wantSomeContext;
        fhicl::Atom<bool> wantSerial;
        fhicl::Atom<bool> wantFullContext;
        fhicl::Atom<bool> wantTimeSeparate;
        fhicl::Atom<bool> wantEpilogueSeparate;
      };

      ~MsgFormatSettings();
      MsgFormatSettings(Config const& config);

      bool want(flag_enum const) const;
      std::string timestamp(timeval const&);

      std::bitset<NFLAGS> flags;
      std::size_t lineLength;

    private:
      int timeMethod_{0};
      std::string timeFmt_{};
    };

    class MsgStatistics {
    public:
      struct Config {
        ~Config();
        Config();
        // FIXME: I think we will want this to apply to resetLimiters() only.
        fhicl::Atom<bool> reset{
          fhicl::Name("reset"),
          fhicl::Comment("Used for statistics destinations"),
          false};
        // FIXME: I think we will want this to apply to resetMsgCounters()
        // only.
        fhicl::Atom<bool> resetStatistics{fhicl::Name("resetStatistics"),
                                          false};
      };

      ~MsgStatistics() = delete;
      MsgStatistics() = delete;
      MsgStatistics(MsgStatistics const&) = delete;
      MsgStatistics(MsgStatistics&&) = delete;
      MsgStatistics& operator=(MsgStatistics const&) = delete;
      MsgStatistics& operator=(MsgStatistics&&) = delete;
    };

    struct Config {
      ~Config();
      Config();
      fhicl::Atom<std::string> dest_type;
      fhicl::Atom<std::string> threshold;
      fhicl::Table<MsgFormatSettings::Config> format;
      fhicl::OptionalDelegatedParameter categories;
      fhicl::Atom<bool> outputStatistics;
      fhicl::TableFragment<MsgStatistics::Config> msgStatistics;
    };

    virtual ~ELdestination() noexcept = default;
    ELdestination(Config const&);
    ELdestination(ELdestination const&) = delete;
    ELdestination(ELdestination&&) = delete;
    ELdestination& operator=(ELdestination const&) = delete;
    ELdestination& operator=(ELdestination&&) = delete;

    // Used only by MessageLoggerScribe to disable cerr_early once startup is
    // complete.
    void setThreshold(ELseverityLevel sv);

    // API which may be overridden by subclasses
    // Currently the only overrider is ELstatistics and then only log() and
    // summary(), not finish()

    // Called only by MessageLoggerScribe::sendMsgToDests
    //   Called by MessageLoggerScribe::logMessage
    //     Called by MessageLogger::LogErrorObj
    //       Called by MaybeLogger_ dtor
    //   Called by MessageLoggerScribe dtor
    //     Called by MessageLogger::EndMessageFacility
    //     Called by global dtor of MessageLogger::mlscribe_ptr
    virtual void log(mf::ErrorObj&);

    // Called only by MessageLoggerScribe dtor.
    // Called at message facility shutdown.
    virtual void finish();

    // Called only by MessageLoggerScribe::summarize()
    //   Called only by MessageLogger::LogStatistics()
    virtual void summary();

  protected:
    std::string summarizeContext(std::string const&);
    void emitToken(std::ostream& os,
                   std::string const& s,
                   bool nl = false,
                   bool preambleMode = false);
    std::string formSummary();

    // A return value of true tells the destination to not log this message.
    // Called by ELdestination::log().
    bool skipMsg(ELextendedID const&);

    // Called by ELdestination::summary()
    // Called by ELstatistics::summary()
    void resetLimiters();

    // Called by ELdestination::summary()
    // Called by ELstatistics::summary()
    void resetMsgCounters();

    virtual void fillPrefix(std::ostringstream&, mf::ErrorObj const& msg);
    virtual void fillUsrMsg(std::ostringstream&, mf::ErrorObj const& msg);
    virtual void fillSuffix(std::ostringstream&, mf::ErrorObj const& msg);
    virtual void routePayload(std::ostringstream const&,
                              mf::ErrorObj const& msg);

    // We never call this, but artdaq mfextensions needs it.
    virtual void flush();

    ELseverityLevel threshold_;
    MsgFormatSettings format_;
    int defaultLimit_;
    int defaultReportEvery_;
    int defaultTimespan_;
    std::map<std::string const, CategoryParams> categoryParams_;
    std::map<ELextendedID const, XidLimiter> xidLimiters_;
    std::map<ELextendedID const, StatsCount> statsMap_;
    bool outputStatistics_{false};
    bool updatedStats_{false};
    bool reset_{false};

  private:
    std::size_t charsOnLine_{};
  };

} // namespace mf::service

namespace cet {
  template <>
  struct PluginTypeDeducer<mf::service::ELdestination> {
    static std::string const value;
  };
} // namespace cet

#endif /* messagefacility_MessageService_ELdestination_h */

// Local variables:
// mode: c++
// End:
