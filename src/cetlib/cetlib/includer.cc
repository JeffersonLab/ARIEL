// ======================================================================
//
// includer: A container-like data structure that
//           a) transparently handles #include'd files, and
//           b) can trace back its iterators
//
// =====================================================================
#include "cetlib/includer.h"

#include "boost/filesystem.hpp"
#include "cetlib/split_by_regex.h"
#include "cetlib/trim.h"
#include "cetlib_except/coded_exception.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

using cet::includer;

namespace bfs = boost::filesystem;

// ----------------------------------------------------------------------

namespace {
  namespace detail {

    enum error { cant_open, cant_read, malformed, recursive };

    std::string
    translate(error code)
    {
      switch (code) {
        case cant_open:
          return "Can't locate or can't open specified file:";
        case cant_read:
          return "Can't read from supplied input stream:";
        case malformed:
          return "Malformed #include directive:";
        case recursive:
          return "Recursive #include directive:";
        default:
          return "Unknown code";
      }
    } // translate()

    using inc_exception = cet::coded_exception<error, translate>;

    std::string
    begin_string()
    {
      return "<begin>";
    }
    std::string
    end_string()
    {
      return "<end>";
    }

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

  } // ::detail.

  std::string
  canonicalizePath(std::string const& path_str) try {
    bfs::path path{path_str};
    // If specified path does not exist, complain later.
    if (bfs::exists(path)) {
      path = bfs::canonical(path);
    }
    std::string result = path.native();
    return result;
  }
  catch (std::exception const& e) {
    throw detail::inc_exception(detail::cant_open)
      << "Exception while examining include specification \"" << path_str
      << "\": " << e.what();
  }

} // anonymous.

using namespace ::detail;

// ----------------------------------------------------------------------

includer::includer(std::string const& filename,
                   cet::filepath_maker& policy_filename)
  : frames{frame(0, begin_string(), 0, text.size())}
{
  include(0, filename, policy_filename);
  frames.emplace_back(0, end_string(), 0, text.size());
}

// ----------------------------------------------------------------------

includer::includer(std::istream& is, cet::filepath_maker& policy_filename)
  : frames{frame(0, begin_string(), 0, text.size())}
{
  include(is, policy_filename);
  frames.emplace_back(0, end_string(), 0, text.size());
}

// ----------------------------------------------------------------------

includer::posinfo
includer::get_posinfo(const_iterator const& it) const
{
  // locate the frame corresponding to the given iterator:
  uint textpos = it - text.begin();
  uint framenum;
  for (framenum = 1u; framenum != frames.size(); ++framenum)
    if (textpos < frames[framenum].starting_textpos)
      break;
  frame const& this_frame = frames[--framenum];

  // determine the line number within the corresponding file:
  uint linenum = this_frame.starting_linenum +
                 std::count(text.begin() + this_frame.starting_textpos,
                            text.begin() + textpos,
                            '\n');

  // determine the character position within the corresponding line:
  uint newlinepos =
    textpos == 0u ? std::string::npos : text.find_last_of('\n', textpos - 1u);
  uint charpos = newlinepos == std::string::npos ?
                   textpos + 1 :
                   textpos - text.find_last_of('\n', textpos - 1u);
  return {textpos, linenum, charpos, framenum};
}

std::string
includer::whereis(const_iterator const& it) const
{
  posinfo const pos(get_posinfo(it));
  // prepare the current information:
  std::ostringstream result;
  result << "line " << pos.linenum << ", character " << pos.charpos
         << ", of file \"" << frames[pos.framenum].filename << '\"';

  return result.str() + backtrace(frames[pos.framenum].including_framenum);

} // whereis()

std::string
includer::highlighted_whereis(const_iterator const& it) const
{
  posinfo const pos(get_posinfo(it));
  // prepare the current information:
  std::ostringstream result;
  result << "line " << pos.linenum << ", character " << pos.charpos
         << ", of file \"" << frames[pos.framenum].filename << '\"'
         << backtrace(frames[pos.framenum].including_framenum) << "\n\n";
  auto cp0 = pos.charpos - 1;
  uint eol = text.find_first_of('\n', pos.textpos);
  result << text.substr(pos.textpos - cp0,
                        (eol == std::string::npos) ?
                          std::string::npos :
                          (eol - (pos.textpos - cp0)))
         << "\n";
  result << std::string(cp0, ' ') << "^";

  return result.str();

} // highlighted_whereis()

std::string
includer::src_whereis(const_iterator const& it) const
{
  posinfo const pos(get_posinfo(it));

  std::ostringstream result;
  result << frames[pos.framenum].filename << ':' << pos.linenum;
  return result.str();

} // src_whereis()

void
includer::include(int including_framenum,
                  std::string const& filename,
                  cet::filepath_maker& policy_filename)
{
  static std::string const inc_lit = std::string("#include");
  static uint const min_sz = inc_lit.size() + 3u;

  // expand filename to obtain, per policy, path to file:
  bool const use_cin = (filename == "-");
  std::string const filepath = use_cin ? filename : policy_filename(filename);
  std::string const canonical_filepath =
    use_cin ? canonicalizePath(filepath) : filepath;

  // check for recursive #inclusion:
  if (std::find(recursion_stack.crbegin(),
                recursion_stack.crend(),
                canonical_filepath) != recursion_stack.crend()) {
    throw inc_exception(recursive)
      << filename << " => " << filepath << backtrace(frames.size() - 1u);
  } else {
    // Record opening of file.
    recursion_stack.emplace_back(canonical_filepath);
  }

  // open the #included file:
  std::ifstream ifs;
  if (!use_cin)
    ifs.open(filepath.c_str(), std::ifstream::in);
  std::istream& f = use_cin ? std::cin : ifs;
  if (!f)
    throw inc_exception(cant_open)
      << filename << " => " << filepath << backtrace(frames.size() - 1u);

  int const starting_linenum = 1;
  frame new_frame(including_framenum, filepath, starting_linenum, text.size());

  int linenum = 0;
  // iterate over each line of the input file:
  for (auto& line : getlines(f)) {

    ++linenum;
    if (line.find(inc_lit) != 0) { // ordinary line (not an #include)
      text.append(line).append(1, '\n');
      continue;
    }

    // save buffered text:
    frames.push_back(new_frame);

    // record this #include's place:
    new_frame.starting_linenum = linenum;
    new_frame.starting_textpos = text.size();
    frames.push_back(new_frame);

    // validate the rest of the #include line's syntax:
    trim_right(line, " \t\r\n");
    if (line.size() <= min_sz                        // too short?
        || line[8] != ' '                            // missing separator?
        || line[9] != '\"' || line.end()[-1] != '\"' // missing either quote?
    )
      throw inc_exception(malformed)
        << line << "\n at line " << linenum << " of file " << filepath;

    // process the #include:
    std::string nextfilename(line.substr(min_sz - 1u, line.size() - min_sz));
    include(frames.size() - 1u, nextfilename, policy_filename);

    // prepare to resume where we left off:
    new_frame.starting_linenum = linenum + 1;
    new_frame.starting_textpos = text.size();
  } // for

  // save final buffered text:
  frames.push_back(new_frame);

  // Done with this file.
  recursion_stack.pop_back();

} // include()

// ----------------------------------------------------------------------

void
includer::include(std::istream& f, cet::filepath_maker& policy_filename)
{
  static std::string const inc_lit = std::string("#include");
  static uint const min_sz = inc_lit.size() + 3u;

  // expand filename to obtain, per policy, absolute path to file:
  std::string const filepath = "-";

  // check the open file:
  if (!f)
    throw inc_exception(cant_open) << filepath << backtrace(frames.size() - 1u);

  int const starting_linenum = 1;
  frame new_frame(0, filepath, starting_linenum, text.size());

  int linenum = 0;
  // iterate over each line of the input file:
  for (auto& line : getlines(f)) {
    ++linenum;
    if (line.find(inc_lit) != 0) { // ordinary line (not an #include)
      text.append(line).append(1, '\n');
      continue;
    }

    // save buffered text:
    frames.push_back(new_frame);

    // record this #include's place:
    new_frame.starting_linenum = linenum;
    new_frame.starting_textpos = text.size();
    frames.push_back(new_frame);

    // validate the rest of the #include line's syntax:
    trim_right(line, " \t\r\n");
    if (line.size() <= min_sz                        // too short?
        || line[8] != ' '                            // missing separator?
        || line[9] != '\"' || line.end()[-1] != '\"' // missing either quote?
    )
      throw inc_exception(malformed)
        << line << "\n at line " << linenum << " of file " << filepath;

    // process the #include:
    std::string nextfilename(line.substr(min_sz - 1u, line.size() - min_sz));
    include(frames.size() - 1u, nextfilename, policy_filename);

    // prepare to resume where we left off:
    new_frame.starting_linenum = linenum + 1;
    new_frame.starting_textpos = text.size();
  } // for

  // save final buffered text:
  frames.push_back(new_frame);

} // include()

// ----------------------------------------------------------------------

std::string
includer::backtrace(uint from_frame) const
{
  std::ostringstream result;
  // append the backtrace:
  for (uint k = from_frame; k != 0u; k = frames[k].including_framenum) {
    result << "\nincluded from line " << frames[k].starting_linenum
           << " of file \"" << frames[k].filename << '\"';
  }

  return result.str();

} // backtrace()

// ----------------------------------------------------------------------

void
includer::debug() const
{
  std::ostringstream result;

  result << "\nframe[" << 0 << "] " << frames[0].including_framenum << "  "
         << frames[0].starting_linenum << "  " << frames[0].starting_textpos
         << "  " << frames[0].filename << '\n';

  for (uint k = 1u; k != frames.size(); ++k) {
    uint starting_textpos = frames[k - 1u].starting_textpos;
    result << "\nframe[" << k << "] " << frames[k].including_framenum << "  "
           << frames[k].starting_linenum << "  " << frames[k].starting_textpos
           << "  " << frames[k].filename << '\n'
           << text.substr(starting_textpos,
                          frames[k].starting_textpos - starting_textpos);
  }
  std::cerr << result.str();
} // debug()

// ======================================================================
