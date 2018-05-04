#include "messagefacility/MessageService/ELdestination.h"
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
#include "messagefacility/Utilities/bold_fontify.h"

#include <bitset>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <map>
#include <ostream>
#include <sstream>
#include <string>

#include <sys/time.h>
#include <time.h>

using namespace std;
using namespace std::string_literals;

string const cet::PluginTypeDeducer<mf::service::ELdestination>::value =
  "ELdestination"s;

namespace mf {
  namespace service {

    ELdestination::Category::Config::~Config() {}

    ELdestination::Category::Config::Config()
      : limit{fhicl::Name{"limit"}, -1}
      , reportEvery{fhicl::Name{"reportEvery"}, -1}
      , timespan{fhicl::Name{"timespan"}, -1}
    {}

    ELdestination::Category::Config::Config(fhicl::ParameterSet const& pset)
      : limit{fhicl::Name{"limit"}, pset.get<int>("limit", -1)}
      , reportEvery{fhicl::Name{"reportEvery"},
                    pset.get<int>("reportEvery", -1)}
      , timespan{fhicl::Name{"timespan"}, pset.get<int>("timespan", -1)}
    {}

    std::string
    ELdestination::Category::Config::limit_comment()
    {
      return R"(The 'limit' parameter is an integer after which the logger will start
to ignore messages of this category.  Beyond the specified limit, an
occasional further message will appear, based on an exponential
fall-off. For example, if the limit is set to 5, and 100 messages of
this category are issued, then the destination containing this
configuration will log messages numbered 1, 2, 3, 4, 5, 10, 15, 25,
45, and 85. A limit of zero disables reporting any messages.)";
    }

    std::string
    ELdestination::Category::Config::reportEvery_comment()
    {
      return R"(The 'reportEvery' parameter is an integer n which logs only every nth
message.  If the value is zero or less, then the report-every feature
is disabled.)";
    }

    std::string
    ELdestination::Category::Config::timespan_comment()
    {
      return R"(The 'timespan' parameter is an integer representing seconds.  When a
limit is set, one can also specify that if no occurrences for that
particular category of messages have been seen in a period of time
(the timespan), then the count toward that limit is to be reset. For
example, if one wish to suppress most of the thousands of warnings of
some category expected at startup, but would like to know if another
one happens after a gap of ten minutes, one can set the timespan value
to 600.  A value of zero or less disables the timespan functionality.)";
    }

    ELdestination::Category::~Category() {}

    ELdestination::Category::Category() {}

    ELdestination::CategoryParams::~CategoryParams() {}

    ELdestination::CategoryParams::CategoryParams()
      : limit_{}, reportEvery_{}, timespan_{}
    {}

    ELdestination::XidLimiter::~XidLimiter() {}

    ELdestination::XidLimiter::XidLimiter()
      : previousTimestamp_{}
      , msgCount_{}
      , skippedMsgCount_{}
      , limit_{}
      , reportEvery_{}
      , timespan_{}
    {}

    ELdestination::MsgFormatSettings::Config::~Config() {}

    ELdestination::MsgFormatSettings::Config::Config()
      : timestamp{fhicl::Name("timestamp"),
                  fhicl::Comment(
                    "The 'timestamp' parameter represents a format that can be "
                    "interpreted\n"
                    "by strftime.  Allowed values include:\n\n"
                    "  - \"none\" (suppress timestamp printing)\n"
                    "  - \"default\" (format string shown below)\n"
                    "  - \"default_ms\" (use millisecond precision)\n"
                    "  - any user-specified format interpretable by strftime"),
                  "%d-%b-%Y %H:%M:%S %Z"}
      , noLineBreaks{fhicl::Name("noLineBreaks"), false}
      , lineLength{fhicl::Name{"lineLength"},
                   fhicl::Comment{"The following parameter is allowed only if "
                                  "'noLineBreaks' has been set to 'false'."},
                   [this] { return !noLineBreaks(); },
                   80ull}
      , wantModule{fhicl::Name("wantModule"), true}
      , wantSubroutine{fhicl::Name("wantSubroutine"), true}
      , wantText{fhicl::Name("wantText"), true}
      , wantSomeContext{fhicl::Name("wantSomeContext"), true}
      , wantSerial{fhicl::Name("wantSerial"), false}
      , wantFullContext{fhicl::Name("wantFullContext"), false}
      , wantTimeSeparate{fhicl::Name("wantTimeSeparate"), false}
      , wantEpilogueSeparate{fhicl::Name("wantEpilogueSeparate"), false}
    {}

    ELdestination::MsgFormatSettings::~MsgFormatSettings() {}

    ELdestination::MsgFormatSettings::MsgFormatSettings(Config const& config)
      : timeMethod_{0}, timeFmt_{}
    {
      auto const& value = config.timestamp();
      bool const use_timestamp = (value != "none"s);
      flags.set(TIMESTAMP, use_timestamp);
      if (!use_timestamp) {
        timeMethod_ = 0;
      } else if (value == "default"s) {
        timeMethod_ = 1;
        timeFmt_ = "%d-%b-%Y %H:%M:%S %Z"s;
      } else if (value == "default_ms"s) {
        timeMethod_ = 2;
        timeFmt_ = "%d-%b-%Y %H:%M:%S.%%03u %Z"s;
      } else {
        timeMethod_ = 1;
        timeFmt_ = value;
      }
      lineLength = config.noLineBreaks() ? 32000ull : config.lineLength();
      flags.set(NO_LINE_BREAKS, config.noLineBreaks());
      flags.set(MODULE, config.wantModule());
      flags.set(SUBROUTINE, config.wantSubroutine());
      flags.set(TEXT, config.wantText());
      flags.set(SOME_CONTEXT, config.wantSomeContext());
      flags.set(SERIAL, config.wantSerial());
      flags.set(FULL_CONTEXT, config.wantFullContext());
      flags.set(TIME_SEPARATE, config.wantTimeSeparate());
      flags.set(EPILOGUE_SEPARATE, config.wantEpilogueSeparate());
    }

    bool
    ELdestination::MsgFormatSettings::want(flag_enum const FLAG) const
    {
      return flags.test(FLAG);
    }

    string
    ELdestination::MsgFormatSettings::timestamp(timeval const& t)
    {
      size_t constexpr SIZE{144};
      string ret;
      if (timeMethod_ == 0) {
        return ret;
      }
      struct tm timebuf;
      char ts[SIZE];
      if (timeMethod_ == 1) {
        strftime(
          ts, sizeof(ts), timeFmt_.data(), localtime_r(&t.tv_sec, &timebuf));
        ret = ts;
        return ret;
      }
      char tmpts[SIZE];
      strftime(tmpts,
               sizeof(tmpts),
               timeFmt_.data(),
               localtime_r(&t.tv_sec, &timebuf));
      snprintf(ts, sizeof(ts), tmpts, static_cast<unsigned>(t.tv_usec / 1000));
      ret = ts;
      return ret;
    }

    ELdestination::StatsCount::~StatsCount() {}

    ELdestination::StatsCount::StatsCount()
      : n_{}
      , aggregateN_{}
      , ignoredFlag_{false}
      , context1_{}
      , context2_{}
      , contextLast_{}
    {}

    void
    ELdestination::StatsCount::add(string const& context, bool const reactedTo)
    {
      if (reactedTo) {
        ++n_;
      }
      ++aggregateN_;
      ((n_ == 1) ? context1_ : (n_ == 2) ? context2_ : contextLast_) =
        string(context, 0, 16);
      if (!reactedTo) {
        ignoredFlag_ = true;
      }
    }

    ELdestination::MsgStatistics::Config::~Config() {}

    ELdestination::MsgStatistics::Config::Config()
      : reset{fhicl::Name("reset"),
              fhicl::Comment("Used for statistics destinations"),
              false}
      , resetStatistics{fhicl::Name("resetStatistics"), false}
    {}

    ELdestination::Config::~Config() {}

    ELdestination::Config::Config()
      : dest_type{fhicl::Name{"type"}}
      , threshold{fhicl::Name{"threshold"},
                  fhicl::Comment{
                    "The 'threshold' parameter specifies the lowest severity "
                    "level of\n"
                    "messages that will be logged to the destination"},
                  "INFO"}
      , format{fhicl::Name{"format"}}
      , categories{fhicl::Name{"categories"},
                   fhicl::Comment{
                     R"(The 'categories' parameter (if provided) is a FHiCL table that
configures the behavior of logging to this destination for the specified
category.  For example, if the following appears in C++ source code:

  mf::LogInfo{"Tracking"} << my_track.diagnostics();

the category is 'Tracking', and its behavior can be specified via:

  categories: {
    Tracking: {
      limit: -1  # default
      reportEvery: -1 # default
      timespan: -1 # default
    }
  }

Within the 'categories' table, it is permitted to specify a 'default'
category, which becomes the configuration for all message categories
that are not explicitly listed.

Note the categories listed only customize the behavior of messages
that are logged specifically to this destination. Messages that are
routed to other destinations are not be affected.

Category parameters
===================

)" + Category::Config::limit_comment() +
                     "\n\n" + Category::Config::reportEvery_comment() + "\n\n" +
                     Category::Config::timespan_comment()}}
      , outputStatistics{fhicl::Name{"outputStatistics"}, false}
      , msgStatistics{}
    {}

    ELdestination::ELdestination(Config const& pset)
      : threshold_{pset.threshold()}
      , format_{pset.format()}
      , defaultLimit_{numeric_limits<decltype(defaultLimit_)>::max()}
      , defaultReportEvery_{-1}
      , defaultTimespan_{numeric_limits<decltype(defaultTimespan_)>::max()}
      , categoryParams_{}
      , xidLimiters_{}
      , statsMap_{}
      , outputStatistics_{pset.outputStatistics()}
      , updatedStats_{false}
      , reset_{pset.msgStatistics().reset() ||
               pset.msgStatistics().resetStatistics()}
      , charsOnLine_{}
    {
      vector<string> configuration_errors;
      // Grab this destination's category configurations.
      fhicl::ParameterSet cats_pset{};
      pset.categories.get_if_present<fhicl::ParameterSet>(cats_pset);
      // Grab the list of categories, removing the default category
      // since it is handled specially.
      auto const default_category_name = "default"s;
      auto categories = cats_pset.get_pset_names();
      auto erase_from =
        remove_if(begin(categories), end(categories), [](auto const& category) {
          return category == "default"s;
        });
      categories.erase(erase_from, categories.cend());
      // Setup the default configuration for categories--this involves
      // resetting the limits table according to the user-specified
      // default configuration.
      auto const& default_pset =
        cats_pset.get<fhicl::ParameterSet>("default"s, {});
      try {
        fhicl::WrappedTable<Category::Config> default_params{default_pset};
        defaultLimit_ = (default_params().limit() < 0) ?
                          numeric_limits<decltype(defaultLimit_)>::max() :
                          default_params().limit();
        defaultReportEvery_ = default_params().reportEvery();
        defaultTimespan_ = (default_params().timespan() < 0) ?
                             numeric_limits<decltype(defaultTimespan_)>::max() :
                             default_params().timespan();
      }
      catch (fhicl::detail::validationException const& e) {
        string msg{"Category: "s + detail::bold_fontify("default"s) + "\n\n"s};
        msg += e.what();
        configuration_errors.push_back(move(msg));
      }
      // Now establish this destination's limit/reportEvery/timespan for
      // each category, using the values of the possibly-specified
      // default configuration when a given category is missing the
      // fields.
      for (auto const& category : categories) {
        fhicl::Table<Category::Config> category_params{fhicl::Name{category},
                                                       default_pset};
        try {
          category_params.validate_ParameterSet(
            cats_pset.get<fhicl::ParameterSet>(category));
        }
        catch (fhicl::detail::validationException const& e) {
          string msg{"Category: " + detail::bold_fontify(category) + "\n\n"};
          msg += e.what();
          configuration_errors.push_back(move(msg));
        }
        // limits_.setCategoryParams(category, category_params().limit(),
        // category_params().reportEvery(), category_params().timespan());
        // setCategoryParams(string const& category, int limit, int reportEvery,
        // int timespan)
        {
          if (category_params().limit() < 0) {
            categoryParams_[category].limit_ = numeric_limits<int>::max();
          } else {
            categoryParams_[category].limit_ = category_params().limit();
          }
          categoryParams_[category].reportEvery_ =
            category_params().reportEvery();
          if (category_params().timespan() < 0) {
            categoryParams_[category].timespan_ = numeric_limits<int>::max();
          } else {
            categoryParams_[category].timespan_ = category_params().timespan();
          }
        }
      }
      if (!configuration_errors.empty()) {
        string msg{"The following categories were misconfigured:\n\n"};
        for (auto const& error : configuration_errors) {
          msg += error;
        }
        throw fhicl::detail::validationException{msg};
      }
    }

    string
    ELdestination::summarizeContext(const string& c)
    {
      if (c.substr(0, 4) != "Run:") {
        return c;
      }
      istringstream is(c);
      string runWord;
      int run;
      is >> runWord >> run;
      if (!is) {
        return c;
      }
      if (runWord != "Run:") {
        return c;
      }
      string eventWord;
      int event;
      is >> eventWord >> event;
      if (!is) {
        return c;
      }
      if (eventWord != "Event:") {
        return c;
      }
      ostringstream os;
      os << run << "/" << event;
      return os.str();
    }

    void
    ELdestination::emitToken(ostream& os,
                             string const& s,
                             bool const nl,
                             bool const preambleMode)
    {
      string indent(6, ' ');
      if (s.empty()) {
        if (nl) {
          os << '\n';
          charsOnLine_ = 0UL;
        }
        return;
      }
      if (!preambleMode) {
        os << s;
        return;
      }
      char const first = s[0];
      char const second = (s.length() < 2) ? '\0' : s[1];
      char const last = (s.length() < 2) ? '\0' : s[s.length() - 1];
      char const last2 = (s.length() < 3) ? '\0' : s[s.length() - 2];
      // checking -2 because the very last char is sometimes a ' '
      // inserted by ErrorLog::operator<<
      // Accounts for newline @ the beginning of the string
      if (first == '\n' || (charsOnLine_ + s.length()) > format_.lineLength) {
        charsOnLine_ = 0UL;
        if (second != ' ') {
          os << ' ';
          ++charsOnLine_;
        }
        if (first == '\n') {
          os << s.substr(1);
        } else {
          os << s;
        }
      } else {
        os << s;
      }
      if ((last == '\n') || (last2 == '\n')) {
        os << indent;
        if (last != ' ') {
          os << ' ';
        }
        charsOnLine_ = indent.length() + 1UL;
      }
      if (nl) {
        os << '\n';
        charsOnLine_ = 0UL;
      } else {
        charsOnLine_ += s.length();
      }
    }

    void
    ELdestination::fillPrefix(ostringstream& oss, ErrorObj const& msg)
    {
      if (msg.is_verbatim()) {
        return;
      }
      auto const& xid = msg.xid();
      charsOnLine_ = 0UL;
      emitToken(oss, "%MSG"s, false, true);
      emitToken(oss, xid.severity().getSymbol(), false, true);
      emitToken(oss, " ", false, true);
      emitToken(oss, xid.id(), false, true);
      emitToken(oss, msg.idOverflow(), false, true);
      emitToken(oss, ": ", false, true);
      if (format_.want(SERIAL)) {
        ostringstream s;
        s << msg.serial();
        emitToken(oss, "[serial #" + s.str() + "] ", false, true);
      }
      bool needAspace = true;
      if (format_.want(EPILOGUE_SEPARATE)) {
        if (xid.module().length() + xid.subroutine().length() > 0) {
          emitToken(oss, "\n", false, true);
          needAspace = false;
        } else if (format_.want(TIMESTAMP) && !format_.want(TIME_SEPARATE)) {
          emitToken(oss, "\n", false, true);
          needAspace = false;
        }
      }
      if (format_.want(MODULE) && (xid.module().length() > 0)) {
        if (needAspace) {
          emitToken(oss, " ", false, true);
          needAspace = false;
        }
        emitToken(oss, xid.module() + " ", false, true);
      }
      if (format_.want(SUBROUTINE) && (xid.subroutine().length() > 0)) {
        if (needAspace) {
          emitToken(oss, " ", false, true);
          needAspace = false;
        }
        emitToken(oss, xid.subroutine() + "() ", false, true);
      }
      if (format_.want(TIMESTAMP)) {
        if (format_.want(TIME_SEPARATE)) {
          emitToken(oss, "\n", false, true);
          needAspace = false;
        }
        if (needAspace) {
          emitToken(oss, " ", false, true);
          needAspace = false;
        }
        emitToken(oss, format_.timestamp(msg.timestamp()) + " ", false, true);
      }
      if (format_.want(SOME_CONTEXT)) {
        if (needAspace) {
          emitToken(oss, " ", false, true);
          needAspace = false;
        }
        emitToken(oss, msg.context(), false, true);
      }
    }

    void
    ELdestination::fillUsrMsg(ostringstream& oss, ErrorObj const& msg)
    {
      if (!format_.want(TEXT)) {
        return;
      }
      // Determine if file and line should be included
      if (!msg.is_verbatim()) {
        if (!msg.filename().empty()) {
          emitToken(oss, " "s);
          emitToken(oss, msg.filename());
          emitToken(oss, ":"s);
          emitToken(oss, to_string(msg.lineNumber()));
        }
        // Check for user-requested line breaks
        if (format_.want(NO_LINE_BREAKS)) {
          emitToken(oss, " ==> ");
        } else {
          emitToken(oss, "", true);
        }
      }
      // Print the contents.
      for (auto const& val : msg.items()) {
        emitToken(oss, val);
      }
    }

    void
    ELdestination::fillSuffix(ostringstream& oss, ErrorObj const& msg)
    {
      if (!msg.is_verbatim() && !format_.want(NO_LINE_BREAKS)) {
        emitToken(oss, "\n%MSG");
      }
      oss << '\n';
    }

    void
    ELdestination::routePayload(ostringstream const&, ErrorObj const&)
    {}

    void
    ELdestination::log(ErrorObj& msg)
    {
      if (msg.xid().severity() < threshold_) {
        if (outputStatistics_) {
          statsMap_[msg.xid()].add(summarizeContext(msg.context()), false);
          updatedStats_ = true;
        }
        return;
      }
      if (skipMsg(msg.xid())) {
        if (outputStatistics_) {
          statsMap_[msg.xid()].add(summarizeContext(msg.context()), false);
          updatedStats_ = true;
        }
        return;
      }
      msg.setReactedTo(true);
      ostringstream payload;
      fillPrefix(payload, msg);
      fillUsrMsg(payload, msg);
      fillSuffix(payload, msg);
      routePayload(payload, msg);
      if (outputStatistics_) {
        statsMap_[msg.xid()].add(summarizeContext(msg.context()), true);
        updatedStats_ = true;
      }
    }

    string
    ELdestination::formSummary()
    {
      ostringstream s;
      int n{};
      // -----  Summary part I:
      //
      bool ftnote{false};
      struct part3 {
        long n{}, t{};
      } p3[ELseverityLevel::nLevels];
      for (auto const& pr : statsMap_) {
        auto const& xid = pr.first;
        auto const& count = pr.second;
        auto const& cat = xid.id();
        // -----  Emit new part I header, if needed:
        //
        if (n == 0) {
          s << "\n";
          s << " type     category        sev    module        subroutine      "
               "  count    total\n"
            << " ---- -------------------- -- ---------------- "
               "----------------  -----    -----\n";
        }
        // -----  Emit detailed message information:
        //
        s
          // Type
          << right << setw(5) << ++n
          << ' '
          // Category
          << left << setw(20) << cat.substr(0, 20)
          << ' '
          // Severity
          << left << setw(2) << xid.severity().getSymbol()
          << ' '
          // Module
          << left << setw(16) << xid.module().substr(0, 16)
          << ' '
          // Subroutine
          << left << setw(16)
          << xid.subroutine().substr(0, 16)
          // Count
          << right << setw(7) << count.n_ << left << setw(1)
          << (count.ignoredFlag_ ? '*' : ' ')
          // Total Count
          << right << setw(8) << count.aggregateN_ << '\n';
        ftnote = ftnote || count.ignoredFlag_;
        // -----  Obtain information for Part III, below:
        //
        p3[xid.severity().getLevel()].n += count.n_;
        p3[xid.severity().getLevel()].t += count.aggregateN_;
      }
      // -----  Provide footnote to part I, if needed:
      //
      if (ftnote) {
        s << "\n* Some occurrences of this message were suppressed in all "
             "logs, due to limits.\n";
      }
      // -----  Summary part II:
      //
      n = 0;
      for (auto const& pr : statsMap_) {
        auto const& xid = pr.first;
        auto const& count = pr.second;
        string const& cat = xid.id();
        if (n == 0) {
          s << '\n'
            << " type    category    Examples: run/evt        run/evt          "
               "run/evt\n"
            << " ---- -------------------- ---------------- ---------------- "
               "----------------\n";
        }
        s
          // Type
          << right << setw(5) << ++n
          << ' '
          // Category
          << left << setw(20) << cat
          << ' '
          // Context1
          << left << setw(16) << count.context1_.c_str()
          << ' '
          // Context2
          << left << setw(16) << count.context2_.c_str()
          << ' '
          // ContextLast
          << count.contextLast_ << '\n';
      }
      // -----  Summary part III:
      //
      s << '\n'
        << "Severity    # Occurrences   Total Occurrences\n"
        << "--------    -------------   -----------------\n";
      for (int k = 0; k < ELseverityLevel::nLevels; ++k) {
        if (p3[k].n != 0 || p3[k].t != 0) {
          s
            // Severity
            << left << setw(8)
            << ELseverityLevel{ELseverityLevel::ELsev_(k)}.getName().c_str()
            // Occurrences
            << right << setw(17)
            << p3[k].n
            // Total Occurrences
            << right << setw(20) << p3[k].t << '\n';
        }
      }
      return s.str();
    }

    // Called only by MessageLoggerScribe::summarize()
    //   Called only by MessageLogger::LogStatistics()
    void
    ELdestination::summary()
    {
      if (outputStatistics_ && updatedStats_) {
        ostringstream payload;
        payload << "\n=============================================\n\n"
                << "MessageLogger Summary\n"
                << formSummary();
        routePayload(payload, ErrorObj{ELzeroSeverity, "No ostream"s});
      }
      updatedStats_ = false;
      if (reset_) {
        resetMsgCounters();
        resetLimiters();
      }
    }

    void
    ELdestination::finish()
    {}

    // Used only by MessageLoggerScribe to disable
    // cerr_early once startup is complete.
    void
    ELdestination::setThreshold(ELseverityLevel const sv)
    {
      threshold_ = sv;
    }

    void
    ELdestination::flush()
    {
      ErrorObj msg{ELwarning, "ELdestination"s};
      msg << "Call to unimplemented flush()!";
      log(msg);
    }

    // A return value of true tells the destination
    // to not log this message.
    // Called by ELdestination log().
    bool
    ELdestination::skipMsg(ELextendedID const& xid)
    {
      auto xl_iter = xidLimiters_.find(xid);
      if (xl_iter == xidLimiters_.end()) {
        // Need to create and initialize a limiter for this xid.
        int limit{defaultLimit_};
        int reportEvery{defaultReportEvery_};
        int timespan{defaultTimespan_};
        // Does the category of this xid have explicit limits?
        auto cp_iter = categoryParams_.find(xid.id());
        if (cp_iter != categoryParams_.end()) {
          // Category limits found, use them.
          auto const& cp = cp_iter->second;
          limit = (cp.limit_ < 0) ? defaultLimit_ : cp.limit_;
          reportEvery =
            (cp.reportEvery_ < 0) ? defaultReportEvery_ : cp.reportEvery_;
          timespan = (cp.timespan_ < 0) ? defaultTimespan_ : cp.timespan_;
        }
        XidLimiter& limiter = xidLimiters_[xid];
        limiter.previousTimestamp_ = time(0);
        limiter.msgCount_ = 0;
        limiter.skippedMsgCount_ =
          reportEvery -
          1; // So that the FIRST of the prescaled messages emerges
        limiter.limit_ = limit;
        limiter.reportEvery_ = reportEvery;
        limiter.timespan_ = timespan;
        xl_iter = xidLimiters_.find(xid);
      }
      XidLimiter& limiter = xl_iter->second;
      time_t now = time(0);
      // Has it been so long that we should restart counting toward the limit?
      if ((limiter.timespan_ >= 0) &&
          (difftime(now, limiter.previousTimestamp_) >= limiter.timespan_)) {
        limiter.msgCount_ = 0;
        if (limiter.reportEvery_ > 0) {
          // So this message will be reacted to
          limiter.skippedMsgCount_ = limiter.reportEvery_ - 1;
        } else {
          limiter.skippedMsgCount_ = 0;
        }
      }
      limiter.previousTimestamp_ = now;
      ++limiter.msgCount_;
      ++limiter.skippedMsgCount_;
      if (limiter.skippedMsgCount_ < limiter.reportEvery_) {
        // Skip message.
        return true;
      }
      if (limiter.limit_ == 0) {
        // Zero limit - never react to this
        // Skip message.
        return true;
      }
      if ((limiter.limit_ < 0) || (limiter.msgCount_ <= limiter.limit_)) {
        limiter.skippedMsgCount_ = 0;
        // Accept message.
        return false;
      }
      // Now we are over the limit - have we exceeded limit by 2^N * limit?
      long diff = limiter.msgCount_ - limiter.limit_;
      long r = diff / limiter.limit_;
      if (r * limiter.limit_ != diff) {
        // Not a multiple of limit - don't react
        // Skip message.
        return true;
      }
      if (r == 1) {
        // Exactly twice limit - react
        limiter.skippedMsgCount_ = 0;
        // Accept message.
        return false;
      }
      while (r > 1) {
        if ((r & 1) != 0) {
          // Not 2**msgCount_ times limit - don't react
          // Skip message.
          return true;
        }
        r >>= 1;
      }
      // If you never get an odd number till one, r is 2**msgCount_ so react
      limiter.skippedMsgCount_ = 0;
      // Accept message.
      return false;
    }

    void
    ELdestination::resetMsgCounters()
    {
      statsMap_.clear();
    }

    void
    ELdestination::resetLimiters()
    {
      xidLimiters_.clear();
    }

  } // namespace service
} // namespace mf
