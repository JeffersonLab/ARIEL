#ifndef cetlib_HorizontalRule_h
#define cetlib_HorizontalRule_h

//====================================================================
// HorizontalRule is used by specifying the length of the rule as a
// c'tor argument, and then by specifying the character to be repeated
// as an argument to the function-call operator (e.g.):
//
//   HorizontalRule r{30}; // Repeats a character 30 times
//   std::cout << r('=') << '\n'
//             << r('-') << '\n';
//
//====================================================================

#include <string>

namespace cet {
  class HorizontalRule {
  public:
    explicit constexpr HorizontalRule(std::size_t const w) : w_{w} {}
    auto
    operator()(char const fill) const
    {
      return std::string(w_, fill);
    }

  private:
    std::size_t const w_;
  };
}

#endif /* cetlib_HorizontalRule_h */

// Local Variables:
// mode: c++
// End:
