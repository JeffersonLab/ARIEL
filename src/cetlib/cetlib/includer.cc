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
  std::regex const reCarriageReturn{"\r"};

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
        for (auto const& line :
             cet::split_by_regex(readline, reCarriageReturn)) {
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

includer::includer(std::string const& filename,
                   cet::filepath_maker& policy_filename)
  : frames_{frame(0, begin_string(), 0, text_.size())}
{
  include(0, filename, policy_filename);
  frames_.emplace_back(0, end_string(), 0, text_.size());
}

includer::includer(std::istream& is, cet::filepath_maker& policy_filename)
  : frames_{frame(0, begin_string(), 0, text_.size())}
{
  include(is, policy_filename);
  frames_.emplace_back(0, end_string(), 0, text_.size());
}

includer::posinfo
includer::get_posinfo(const_iterator const& it) const
{
  // locate the frame corresponding to the given iterator:
  uint textpos = it - text_.begin();
  uint framenum;
  for (framenum = 1u; framenum != frames_.size(); ++framenum)
    if (textpos < frames_[framenum].starting_textpos)
      break;
  frame const& this_frame = frames_[--framenum];

  // determine the line number within the corresponding file:
  uint delta_line_num{};
  if (this_frame.starting_textpos != textpos) {
    auto const this_frame_begin = this_frame.nl_positions.cbegin();
    auto const this_frame_end = this_frame.nl_positions.cend();
    auto const lower =
      std::upper_bound(this_frame_begin, this_frame_end, textpos);
    delta_line_num = std::distance(this_frame_begin, lower);
  }
  uint const linenum = this_frame.starting_linenum + delta_line_num;

  // determine the character position within the corresponding line:
  uint const newlinepos =
    textpos == 0u ? std::string::npos : text_.find_last_of('\n', textpos - 1u);
  uint const charpos = newlinepos == std::string::npos ?
                         textpos + 1 :
                         textpos - text_.find_last_of('\n', textpos - 1u);
  return {textpos, linenum, charpos, framenum};
}

std::string
includer::whereis(const_iterator const& it) const
{
  posinfo const pos{get_posinfo(it)};
  // prepare the current information:
  std::ostringstream result;
  result << "line " << pos.linenum << ", character " << pos.charpos
         << ", of file \"" << frames_[pos.framenum].filename << '\"';

  return result.str() + backtrace(frames_[pos.framenum].including_framenum);
}

std::string
includer::highlighted_whereis(const_iterator const& it) const
{
  posinfo const pos{get_posinfo(it)};
  // prepare the current information:
  std::ostringstream result;
  result << "line " << pos.linenum << ", character " << pos.charpos
         << ", of file \"" << frames_[pos.framenum].filename << '\"'
         << backtrace(frames_[pos.framenum].including_framenum) << "\n\n";
  auto cp0 = pos.charpos - 1;
  uint eol = text_.find_first_of('\n', pos.textpos);
  result << text_.substr(pos.textpos - cp0,
                         (eol == std::string::npos) ?
                           std::string::npos :
                           (eol - (pos.textpos - cp0)))
         << "\n";
  result << std::string(cp0, ' ') << "^";

  return result.str();
}

std::string
includer::src_whereis(const_iterator const& it) const
{
  posinfo const pos{get_posinfo(it)};

  std::ostringstream result;
  result << frames_[pos.framenum].filename << ':' << pos.linenum;
  return result.str();
}

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
  if (std::find(recursionStack_.crbegin(),
                recursionStack_.crend(),
                canonical_filepath) != recursionStack_.crend()) {
    throw inc_exception(recursive)
      << filename << " => " << filepath << backtrace(frames_.size() - 1u);
  } else {
    // Record opening of file.
    recursionStack_.emplace_back(canonical_filepath);
  }

  // open the #included file:
  std::ifstream ifs;
  if (!use_cin)
    ifs.open(filepath.c_str(), std::ifstream::in);
  std::istream& f = use_cin ? std::cin : ifs;
  if (!f)
    throw inc_exception(cant_open)
      << filename << " => " << filepath << backtrace(frames_.size() - 1u);

  int const starting_linenum = 1;
  frame new_frame(including_framenum, filepath, starting_linenum, text_.size());

  int linenum = 0;
  // iterate over each line of the input file:
  for (auto& line : getlines(f)) {
    ++linenum;
    if (line.find(inc_lit) != 0) { // ordinary line (not an #include)
      text_.append(line).append(1, '\n');
      new_frame.nl_positions.push_back(text_.size()); // Record newline
      continue;
    }

    // save buffered text:
    frames_.push_back(new_frame);

    // record this #include's place:
    new_frame.starting_linenum = linenum;
    new_frame.starting_textpos = text_.size();
    frames_.push_back(new_frame);

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
    include(frames_.size() - 1u, nextfilename, policy_filename);

    // prepare to resume where we left off:
    new_frame.starting_linenum = linenum + 1;
    new_frame.starting_textpos = text_.size();
  } // for

  // save final buffered text:
  frames_.push_back(new_frame);

  // Done with this file.
  recursionStack_.pop_back();
}

void
includer::include(std::istream& f, cet::filepath_maker& policy_filename)
{
  static std::string const inc_lit = std::string("#include");
  static uint const min_sz = inc_lit.size() + 3u;

  // expand filename to obtain, per policy, absolute path to file:
  std::string const filepath = "-";

  // check the open file:
  if (!f)
    throw inc_exception(cant_open)
      << filepath << backtrace(frames_.size() - 1u);

  int const starting_linenum = 1;
  frame new_frame(0, filepath, starting_linenum, text_.size());

  int linenum = 0;
  // iterate over each line of the input file:
  for (auto& line : getlines(f)) {
    ++linenum;
    if (line.find(inc_lit) != 0) { // ordinary line (not an #include)
      text_.append(line).append(1, '\n');
      new_frame.nl_positions.push_back(text_.size()); // Record newline
      continue;
    }

    // save buffered text:
    frames_.push_back(new_frame);

    // record this #include's place:
    new_frame.starting_linenum = linenum;
    new_frame.starting_textpos = text_.size();
    frames_.push_back(new_frame);

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
    include(frames_.size() - 1u, nextfilename, policy_filename);

    // prepare to resume where we left off:
    new_frame.starting_linenum = linenum + 1;
    new_frame.starting_textpos = text_.size();
  } // for

  // save final buffered text:
  frames_.push_back(new_frame);
}

std::string
includer::backtrace(uint const from_frame) const
{
  std::ostringstream result;
  // append the backtrace:
  for (uint k = from_frame; k != 0u; k = frames_[k].including_framenum) {
    result << "\nincluded from line " << frames_[k].starting_linenum
           << " of file \"" << frames_[k].filename << '\"';
  }
  return result.str();
}

void
includer::debug() const
{
  std::ostringstream result;

  result << "\nframe[" << 0 << "] " << frames_[0].including_framenum << "  "
         << frames_[0].starting_linenum << "  " << frames_[0].starting_textpos
         << "  " << frames_[0].filename << '\n';

  for (uint k = 1u; k != frames_.size(); ++k) {
    uint starting_textpos = frames_[k - 1u].starting_textpos;
    result << "\nframe[" << k << "] " << frames_[k].including_framenum << "  "
           << frames_[k].starting_linenum << "  " << frames_[k].starting_textpos
           << "  " << frames_[k].filename << '\n'
           << text_.substr(starting_textpos,
                           frames_[k].starting_textpos - starting_textpos);
  }
  std::cerr << result.str();
}
