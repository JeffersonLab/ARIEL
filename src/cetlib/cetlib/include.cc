// ======================================================================
//
// include: File->string, interpolating #include'd files along the way
//
// ======================================================================

#include "cetlib/include.h"
#include "cetlib/split_by_regex.h"

#include "cetlib/filesystem.h"
#include "cetlib/search_path.h"
#include "cetlib/trim.h"
#include "cetlib_except/coded_exception.h"
#include <fstream>

// ----------------------------------------------------------------------

namespace cet {
  namespace detail {

    enum error { cant_open, cant_read, malformed };

    std::string
    translate(error const code)
    {
      switch (code) {
        case cant_open:
          return "Can't locate or can't open specified file:";
        case cant_read:
          return "Can't read from supplied input stream:";
        case malformed:
          return "Malformed #include directive:";
        default:
          return "Unknown code";
      }
    }

    using include_exception = cet::coded_exception<error, translate>;

    std::string const include_lit{"#include \""};
    std::size_t const include_sz{include_lit.size()};

    std::vector<std::string>
    getlines(std::istream& is)
    {
      std::vector<std::string> result;
      for (std::string readline; std::getline(is, readline);) {
        for (auto const& line : cet::split_by_regex(readline, "\r")) {
          result.emplace_back(line);
        }
      }
      return result;
    }

  } // detail
} // cet

using namespace cet::detail;

// ----------------------------------------------------------------------

void
cet::include(std::istream& in, std::string& result)
{
  if (!in)
    throw include_exception{cant_read};

  for (auto& line : getlines(in)) {
    if (line.find(include_lit) != 0) { // ordinary line
      result.append(line).append(1, '\n');
      continue;
    }
    trim_right(line, "\t\r");
    if (line.end()[-1] != '\"') // #include is missing trailing quote
      throw include_exception{malformed} << line;

    std::string const fname{
      line.substr(include_sz, line.size() - include_sz - 1)};
    std::ifstream f{fname.c_str(), std::ios_base::in};
    if (!f)
      throw include_exception{cant_open} << fname;
    include(f, result);
  } // for

} // include()

// ----------------------------------------------------------------------

void
cet::include(std::istream& in,
             std::string const& search_path_arg,
             std::string& result)
{
  cet::search_path const paths{search_path_arg};

  if (!in)
    throw include_exception(cant_read);

  for (auto const& line : getlines(in)) {
    if (line.find(include_lit) != 0) { // ordinary line
      result.append(line).append(1, '\n');
      continue;
    }

    if (line.end()[-1] != '\"') // #include is missing its trailing quote
      throw include_exception(malformed) << line;

    std::string const fname{
      line.substr(include_sz, line.size() - include_sz - 1)};

    if (is_absolute_filepath(fname)) {
      std::ifstream f{fname.c_str(), std::ios_base::in};
      if (!f)
        throw include_exception(cant_open) << fname;
      include(f, search_path_arg, result);
    } else {
      std::ifstream f{paths.find_file(fname).c_str(), std::ios_base::in};
      if (!f) {
        throw include_exception(cant_open)
          << fname << "\nusing path: " << paths;
      }
      include(f, search_path_arg, result);
    }
  } // for

} // include()

// ======================================================================
