// vim: set sw=2 :
#include "canvas/Utilities/EventIDMatcher.h"

#include "canvas/Persistency/Provenance/EventID.h"
#include "canvas/Utilities/Exception.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace art {

  EventIDMatcher::EventIDMatcher(std::string const& pattern)
    : pattern_(), parsed_patterns_()
  {
    if (pattern.empty()) {
      throw art::Exception(errors::LogicError)
        << "EventIDMatcher given an empty pattern!";
    }
    pattern_.push_back(pattern);
    parsed_patterns_.resize(1);
    parse_pattern();
  }

  EventIDMatcher::EventIDMatcher(std::vector<std::string> const& patterns)
    : pattern_(), parsed_patterns_()
  {
    if (patterns.size() == 0) {
      throw art::Exception(errors::LogicError)
        << "EventIDMatcher given an empty list of patterns!";
    }
    pattern_.reserve(patterns.size());
    parsed_patterns_.resize(patterns.size());
    for (auto const& val : patterns) {
      if (val.empty()) {
        throw art::Exception(errors::LogicError)
          << "EventIDMatcher given an empty pattern!";
      }
      pattern_.push_back(val);
    }
    parse_pattern();
  }

  void
  EventIDMatcher::parse_pattern()
  {
    regex pat( // matches ('*' /*wildcard*/ | digits /*single*/ | digits -
               // digits /*range*/)(',' /*list*/ | ':' /*part*/ | eol)
      "("      // 1
      "[[:blank:]]*"
      "("         // 2
      "([*])|"    // 3
      "([0-9]+)|" // 4
      "([0-9]+)"  // 5
      "[[:blank:]]*-[[:blank:]]*"
      "([0-9]+)" // 6
      ")"
      "[[:blank:]]*"
      "([,:]|$)" // 7
      ")");
    int patno = -1;
    for (auto const& given_pattern : pattern_) {
      ++patno;
      regex_iterator<string::const_iterator> I(
        given_pattern.cbegin(), given_pattern.cend(), pat);
      regex_iterator<string::const_iterator> E;
      auto prev_pos = 0L;
      auto prev_len = 0L;
      char prev_sep = '\0';
      parsed_patterns_[patno].resize(3);
      // Note: 0: run, 1: subrun, 2: event
      auto part_num = 0U;
      for (; I != E; ++I) {
        auto const& m = *I;
        char sep = '\0';
        if (m.str(7).size()) {
          sep = m.str(7)[0];
        }
        // printf("%2ld, %2ld: ", m.position(), m.position() + m.length() - 1L);
        if (m.position() != (prev_pos + prev_len)) {
          // err, non-matching characters between
          ostringstream buf;
          buf << '\n';
          buf << "Illegal character in pattern near here:\n";
          buf << given_pattern;
          buf << '\n';
          for (auto i = 0L; i < (prev_pos + prev_len); ++i) {
            buf << ' ';
          }
          buf << "^\n";
          throw art::Exception(errors::LogicError) << buf.str();
        }
        if (m[3].matched) {
          // wildcard
          // printf("%s", m.str(3).c_str());
          parsed_patterns_[patno][part_num].emplace_back(0U, 0U, true);
        } else if (m[4].matched) {
          // single num
          // printf("%s", m.str(4).c_str());
          auto num = 0U;
          for (auto val : m.str(4)) {
            num = (num * 10U) + (val - '0');
          }
          parsed_patterns_[patno][part_num].emplace_back(num, num, false);
        } else {
          // range
          // printf("%s-%s", m.str(5).c_str(), m.str(6).c_str());
          auto num_low = 0U;
          for (auto val : m.str(5)) {
            num_low = (num_low * 10U) + (val - '0');
          }
          auto num_high = 0U;
          for (auto val : m.str(6)) {
            num_high = (num_high * 10U) + (val - '0');
          }
          parsed_patterns_[patno][part_num].emplace_back(
            num_low, num_high, false);
        }
        // printf("%s\n", m.str(7).c_str());
        if (sep == ':') {
          if (part_num == 0U) {
            // at end of run part
          } else if (part_num == 1U) {
            // at end of subrun part
          } else if (part_num == 2U) {
            // error, event part ended with a ':'
            ostringstream buf;
            buf << '\n';
            buf
              << "Syntax error, event part of pattern ended with a ':' here:\n";
            buf << given_pattern;
            buf << '\n';
            for (auto i = 0L; i < m.position(7); ++i) {
              buf << ' ';
            }
            buf << "^\n";
            throw art::Exception(errors::LogicError) << buf.str();
          }
          ++part_num;
        } else if (sep == ',') {
          // range continues
        } else {
          // at end of event part, and end of string
        }
        prev_pos = m.position();
        prev_len = m.length();
        prev_sep = sep;
      }
      if (prev_sep != '\0') {
        // err, last match did not finish properly
        ostringstream buf;
        buf << '\n';
        printf("pep: Syntax error, near here:\n");
        buf << given_pattern;
        buf << '\n';
        for (auto i = 0L; i < (prev_pos + prev_len); ++i) {
          buf << ' ';
        }
        buf << "^\n";
        throw art::Exception(errors::LogicError) << buf.str();
      }
      if (static_cast<string::size_type>(prev_pos + prev_len) !=
          given_pattern.size()) {
        // err, did not match whole string
        ostringstream buf;
        buf << '\n';
        printf("pep: Syntax error, near here:\n");
        buf << given_pattern;
        buf << '\n';
        for (auto i = 0L; i < (prev_pos + prev_len); ++i) {
          buf << ' ';
        }
        buf << "^\n";
        throw art::Exception(errors::LogicError) << buf.str();
      }
      // for (auto i = 0U; i < 3; ++i) {
      //  printf("-----\n");
      //  for (auto const& val : parsed_patterns_[patno][i]) {
      //    if (val.wildcard_) {
      //      printf("*\n");
      //    }
      //    else if (val.low_ == val.high_) {
      //      printf("%u\n", val.low_);
      //    }
      //    else {
      //      printf("%u-%u\n", val.low_, val.high_);
      //    }
      //  }
      //}
    }
  }

  bool
  EventIDMatcher::operator()(EventID const& eid) const
  {
    return match(eid);
  }

  bool
  EventIDMatcher::match(EventID const& eid) const
  {
    bool ret = false;
    //*/printf("\nMatching pattern: %s\n", pattern_.c_str());
    if (!eid.isValid() || eid.isFlush()) {
      //*/printf("Decision: %d\n\n", ret);
      return ret;
    }
    if (!eid.subRunID().isValid() || eid.subRunID().isFlush()) {
      //*/printf("Decision: %d\n\n", ret);
      return ret;
    }
    if (!eid.runID().isValid() || eid.runID().isFlush()) {
      //*/printf("Decision: %d\n\n", ret);
      return ret;
    }
    for (auto const& parsed_pattern : parsed_patterns_) {
      for (auto i = 0U; i < 3; ++i) {
        //*/printf("----- ");
        //*/if (i == 0U) {
        //*/printf("   run part -- ");
        //*/}
        //*/else if (i == 1U) {
        //*/printf("subrun part -- ");
        //*/}
        //*/else {
        //*/printf(" event part -- ");
        //*/}
        //*/printf("run: %u  subRun: %u  event: %u\n", eid.run(), eid.subRun(),
        // eid.event());
        for (auto const& val : parsed_pattern[i]) {
          if (val.wildcard_) {
            // Wildcards always match
            //*/printf("*: matched\n");
            ret = true;
            break;
          } else if (val.low_ == val.high_) {
            // Single value match
            if (i == 0U) {
              // run
              if (eid.run() == val.low_) {
                //*/printf("%u: matched\n", val.low_);
                ret = true;
                break;
              } else {
                //*/printf("%u: no match\n", val.low_);
              }
            } else if (i == 1U) {
              // subrun
              if (eid.subRun() == val.low_) {
                //*/printf("%u: matched\n", val.low_);
                ret = true;
                break;
              } else {
                //*/printf("%u: no match\n", val.low_);
              }
            } else {
              // event
              if (eid.event() == val.low_) {
                //*/printf("%u: matched\n", val.low_);
                ret = true;
                break;
              } else {
                //*/printf("%u: no match\n", val.low_);
              }
            }
          } else {
            // Range match
            //*/printf("%u-%u\n", val.low_, val.high_);
            if (i == 0U) {
              // run
              if ((eid.run() >= val.low_) && (eid.run() <= val.high_)) {
                //*/printf("%u-%u: matched\n", val.low_, val.high_);
                ret = true;
                break;
              } else {
                //*/printf("%u-%u: no match\n", val.low_, val.high_);
              }
            } else if (i == 1U) {
              // subrun
              if ((eid.subRun() >= val.low_) && (eid.subRun() <= val.high_)) {
                //*/printf("%u-%u: matched\n", val.low_, val.high_);
                ret = true;
                break;
              } else {
                //*/printf("%u-%u: no match\n", val.low_, val.high_);
              }
            } else {
              // event
              if ((eid.event() >= val.low_) && (eid.event() <= val.high_)) {
                //*/printf("%u-%u: matched\n", val.low_, val.high_);
                ret = true;
                break;
              } else {
                //*/printf("%u-%u: no match\n", val.low_, val.high_);
              }
            }
          }
        }
        if (!ret) {
          // nothing matched
          break;
        }
        if (i != 2U) {
          // Reset for next part.
          ret = false;
        }
      }
      //*/printf("Decision: %d\n\n", ret);
      if (ret) {
        // We matched, return immediately.
        return ret;
      }
    }
    // We never matched.
    return ret;
  }

} // namespace art
