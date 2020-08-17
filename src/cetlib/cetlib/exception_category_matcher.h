#ifndef cetlib_exception_category_matcher_h
#define cetlib_exception_category_matcher_h
////////////////////////////////////////////////////////////////////////
// exception_category_matcher
//
// Utility for matching cet::exception categories when one is using the
// Catch unit testing package.
//
// Initialize with the string representation of the category to be
// matched.
//
// Composable with other bases of Catch::MatcherBase<cet::exception>
// such as cet::exception_message_matcher.
////////////////////////////////////////////////////////////////////////
#include "cetlib_except/exception.h"
#include <catch2/catch.hpp>

#include <string>

namespace cet {
  class exception_category_matcher;
}

class cet::exception_category_matcher
  : public Catch::MatcherBase<cet::exception> {
public:
  explicit exception_category_matcher(std::string category)
    : category_{std::move(category)}
  {}

  bool
  match(cet::exception const& e) const override
  {
    return e.category() == category_;
  }

  std::string
  describe() const override
  {
    using namespace std::string_literals;
    std::string desc{"is of category \""s + category_ + '"'};
    return desc;
  }

private:
  std::string category_;
};

#endif /* cetlib_exception_category_matcher_h */

// Local Variables:
// mode: c++
// End:
