#ifndef cetlib_bold_fontify_h
#define cetlib_bold_fontify_h

#include <string>

namespace cet {
  inline std::string
  bold_fontify(std::string const& s)
  {
    return "\033[1m" + s + "\033[0m";
  }
}

#endif /* cetlib_bold_fontify_h */

// Local variables:
// Mode: c++
// End:
