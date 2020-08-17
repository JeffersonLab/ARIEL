#ifndef cetlib_exception_message_matcher_h
#define cetlib_exception_message_matcher_h
////////////////////////////////////////////////////////////////////////
// exception_message_matcher
//
// Utility for matching cet::exception messages when one is using the
// Catch unit testing package.
//
// Initialize with any valid dstring matcher function (Contains(),
// EndsWith(), Regex(), etc.).
//
// Composable with other bases of Catch::MatcherBase<cet::exception>
// such as cet::exception_category_matcher.
////////////////////////////////////////////////////////////////////////
#include "cetlib_except/exception.h"
#include <catch2/catch.hpp>

#include <memory>
#include <string>

namespace cet {
  class exception_message_matcher;
}

class cet::exception_message_matcher
  : public Catch::MatcherBase<cet::exception> {
public:
  exception_message_matcher(
    Catch::Matchers::StdString::EqualsMatcher const& matcher)
    : matcher_{std::make_shared<Catch::Matchers::StdString::EqualsMatcher>(
        matcher)}
    , description_{matcher.describe()}
  {}

  exception_message_matcher(
    Catch::Matchers::StdString::ContainsMatcher const& matcher)
    : matcher_{std::make_shared<Catch::Matchers::StdString::ContainsMatcher>(
        matcher)}
    , description_{matcher.describe()}
  {}

  exception_message_matcher(
    Catch::Matchers::StdString::StartsWithMatcher const& matcher)
    : matcher_{std::make_shared<Catch::Matchers::StdString::StartsWithMatcher>(
        matcher)}
    , description_{matcher.describe()}
  {}

  exception_message_matcher(
    Catch::Matchers::StdString::EndsWithMatcher const& matcher)
    : matcher_{std::make_shared<Catch::Matchers::StdString::EndsWithMatcher>(
        matcher)}
    , description_{matcher.describe()}
  {}

  exception_message_matcher(
    Catch::Matchers::StdString::RegexMatcher const& matcher)
    : matcher_{std::make_shared<Catch::Matchers::StdString::RegexMatcher>(
        matcher)}
    , description_{matcher.describe()}
  {}

  bool
  match(cet::exception const& e) const override
  {
    return matcher_->match(e.what());
  }

  std::string
  describe() const override
  {
    return description_;
  }

private:
  std::shared_ptr<Catch::StringMatcher> matcher_;
  std::string description_;
};

#endif /* cetlib_exception_message_matcher_h */

// Local Variables:
// mode: c++
// End:
