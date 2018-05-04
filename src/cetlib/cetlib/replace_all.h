#ifndef cetlib_replace_all_h
#define cetlib_replace_all_h

#include <string>

namespace cet {
  ///
  /// \brief Replace all occurrences of from in string with to.
  ///
  /// \returns true if anything has been done to in.
  ///
  /// \param[in,out] in The string to be manipulated.
  /// \param[in] from The string to be replaced.
  /// \param[in] to The replacement of from.
  bool replace_all(std::string& in,
                   std::string const& from,
                   std::string const& to);
}

#endif /* cetlib_replace_all_h */

// Local variables:
// mode:c++
// End:
