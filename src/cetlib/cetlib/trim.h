#ifndef cetlib_trim_h
#define cetlib_trim_h

// ======================================================================
//
// trim: Remove a string's leading/trailing whitespace
//
// ======================================================================

#include <string>

namespace cet {

  // trim in place:
  inline std::string& trim_right(std::string& source,
                                 std::string const& t = " ");
  inline std::string& trim_left(std::string& source,
                                std::string const& t = " ");
  inline std::string& trim(std::string& source, std::string const& t = " ");

  // trim and return a copy:
  inline std::string trim_right_copy(std::string source,
                                     std::string const& t = " ");
  inline std::string trim_left_copy(std::string source,
                                    std::string const& t = " ");
  inline std::string trim_copy(std::string source, std::string const& t = " ");
}

// ----------------------------------------------------------------------
// trim in place

std::string&
cet::trim_right(std::string& source, std::string const& t)
{
  return source.erase(1 + source.find_last_not_of(t));
}

std::string&
cet::trim_left(std::string& source, std::string const& t)
{
  return source.erase(0, source.find_first_not_of(t));
}

std::string&
cet::trim(std::string& source, std::string const& t)
{
  return trim_right(source, t), trim_left(source, t);
}

// ----------------------------------------------------------------------
// trim and return a copy

std::string
cet::trim_right_copy(std::string source, std::string const& t)
{
  return trim_right(source, t);
}

std::string
cet::trim_left_copy(std::string source, std::string const& t)
{
  return trim_left(source, t);
}

std::string
cet::trim_copy(std::string source, std::string const& t)
{
  return trim(source, t);
}

// ======================================================================

#endif /* cetlib_trim_h */

// Local variables:
// mode: c++
// End:
