#ifndef messagefacility_Utilities_bold_fontify_h
#define messagefacility_Utilities_bold_fontify_h

#include <string>

namespace mf {
  namespace detail {
    inline std::string
    bold_fontify(std::string const& s)
    {
      return "\033[1m" + s + "\033[0m";
    }
  }
}

#endif /* messagefacility_Utilities_bold_fontify_h */

// Local variables:
// Mode: c++
// End:
