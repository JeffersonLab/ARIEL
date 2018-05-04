// ======================================================================
//
// canonical_string: Transform a string into a canonical form
//
// ======================================================================

#include "cetlib/canonical_string.h"

#include "cetlib_except/exception.h"

#include <cctype>

// ----------------------------------------------------------------------

std::string
cet::escape(std::string const& str)
{
  std::string result;
  for (std::string::const_iterator it = str.begin(), e = str.end(); it != e;
       ++it) {
    switch (*it) {
      case '\"':
        result.append("\\\"");
        break;
      case '\'':
        result.append("\\\'");
        break;
      case '\\':
        result.append("\\\\");
        break;
      case '\n':
        result.append("\\n");
        break;
      case '\t':
        result.append("\\t");
        break;
      default:
        if (std::isprint(*it))
          result.append(1, *it);
        else
          throw cet::exception("unprintable character");
    }
  }
  return result;
} // escape()

// ----------------------------------------------------------------------

std::string
cet::unescape(std::string const& str)
{
  std::string result;
  for (std::string::const_iterator it = str.begin(), e = str.end(); it != e;
       ++it) {
    char ch = *it;
    if (ch == '\\' && it != e - 1) {
      switch (*++it) {
        case '\"':
          ch = '\"';
          break;
        case '\'':
          ch = '\'';
          break;
        case '\\':
          ch = '\\';
          break;
        case 'n':
          ch = '\n';
          break;
        case 't':
          ch = '\t';
          break;
        default:
          throw cet::exception("unknown escape: \\") << *it;
      }
    }
    result.append(1, ch);
  }
  return result;
} // unescape()

// ----------------------------------------------------------------------

std::string
cet::canonical_string(std::string const& s)
{
  std::string result;
  if (!s.empty()) {
    std::string value;
    if (is_double_quoted_string(s)) {
      value = cet::unescape(s.substr(1, s.size() - 2));
    } else if (is_single_quoted_string(s)) {
      value = s.substr(1, s.size() - 2);
    } else {
      value = s;
    }

    result.append(1, '"').append(cet::escape(value)).append(1, '"');
  }
  return result;
}

bool
cet::detail::is_quoted_string(std::string const& s, char quot)
{
  bool result = false;
  auto const sz = s.size();
  if ((sz >= 2) && (s[0] == quot) && (s.back() == quot)) {
    result = true;
  }
  return result;
}

// ======================================================================
