#ifndef cetlib_canonical_string_h
#define cetlib_canonical_string_h

// ======================================================================
//
// canonical_string: Transform a string into a canonical form
//
// ======================================================================

#include <string>

namespace cet {

  // Escape a string.
  std::string escape(std::string const& str);

  // Unescape a string.
  std::string unescape(std::string const& str);

  // Check quoting for string.
  bool is_single_quoted_string(std::string const& str);
  bool is_double_quoted_string(std::string const& str);
  bool is_quoted_string(std::string const& str); // Either single or double.,

  // Cannot throw; returns false on error or empty.
  // N.B. appends to result.
  bool canonical_string(std::string const& str, std::string& result);

  // Throw on error; returns empty string on empty.
  std::string canonical_string(std::string const& str);

  // Underlying function for internal use:
  namespace detail {
    // Check if string is quoted by the specified character.
    bool is_quoted_string(std::string const& s, char quot);
  }
}

inline bool
cet::is_single_quoted_string(std::string const& str)
{
  return detail::is_quoted_string(str, '\'');
}

inline bool
cet::is_double_quoted_string(std::string const& str)
{
  return detail::is_quoted_string(str, '"');
}

inline bool
cet::is_quoted_string(std::string const& str)
{
  return is_single_quoted_string(str) || is_double_quoted_string(str);
}

inline bool
cet::canonical_string(std::string const& s, std::string& result) try {
  bool success = false;
  if (!s.empty()) {
    result.append(canonical_string(s));
    success = true;
  }
  return success;
}
catch (...) {
  return false;
} // canonical_string()

// ======================================================================

#endif /* cetlib_canonical_string_h */

// Local Variables:
// mode: c++
// End:
