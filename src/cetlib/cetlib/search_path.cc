// ======================================================================
//
// search_path: Seek filename or pattern in a given list of pathnames
//
// ======================================================================

#include "cetlib/search_path.h"
#include "cetlib/filesystem.h"
#include "cetlib/getenv.h"
#include "cetlib_except/exception.h"

#include <dirent.h>
#include <iterator>
#include <ostream>
#include <regex>
#include <algorithm>
#include <sys/stat.h>

using namespace std;
using cet::search_path;

namespace {
  string const exception_category{"search_path"};

  string
  get_env_if_present(string const& arg)
  {
    // If no colon and no slash is present, assume the user is specifying an
    // environment variable.
    return (arg.find(':') == string::npos && arg.find('/') == string::npos )
      ? arg : string{};
  }

  vector<string>
  get_dirs(std::string const& env, std::string const& paths)
  {
    string const& path_to_split = env.empty() ? paths : cet::getenv(env);

    vector<string> dirs;
    cet::split(path_to_split, ':', back_inserter(dirs));

    if (dirs.empty()) {
      dirs.emplace_back();
    } else {
      // Delete trailing slashes
      for_each(dirs.begin(), dirs.end(), [](string& s) {
	  if (s.size()>1 && s.back() == '/')
	    s.erase(s.size()-1);
	});
      // Remove duplicates while preserving order
      for (auto it = dirs.begin(); it != dirs.end(); ++it ) {
	for (auto jt = it+1; (jt = find(jt, dirs.end(), *it)) != dirs.end();) {
	  jt = dirs.erase(jt);
	}
      }
    }
    return dirs;
  }
}

search_path::search_path(string const& arg)
  : env_{get_env_if_present(arg)}, dirs_{get_dirs(env_, arg)}
{}

bool
search_path::empty() const
{
  return dirs_.empty();
}

size_t
search_path::size() const
{
  return dirs_.size();
}

string const& search_path::operator[](size_t const k) const
{
  return dirs_.at(k);
}

// ----------------------------------------------------------------------
// find_file() overloads:

string
search_path::find_file(string const& filename) const
{
  string result;
  if (find_file(filename, result))
    return result;
  throw cet::exception(exception_category)
    << "Can't find file \"" << filename << '"';
}

bool
search_path::find_file(string const& filename, string& result) const
{
  if (filename.empty())
    return false;

  for (auto const& dir : dirs_) {
    string fullpath = dir + '/' + filename;
    for (size_t k; (k = fullpath.find("//")) != string::npos;) {
      fullpath.erase(k, 1);
    }
    if (cet::file_exists(fullpath)) {
      result = fullpath;
      return true;
    }
  }
  return false;
}

// ----------------------------------------------------------------------
size_t
search_path::find_files(string const& pat, vector<string>& out) const
{
  regex const re{pat};

  size_t count{};
  size_t err{};
  struct dirent entry;
  struct dirent* result = nullptr;

  for (auto const& dir : dirs_) {
    unique_ptr<DIR, function<int(DIR*)>> dd(opendir(dir.c_str()), closedir);
    if (dd == nullptr)
      continue;
    while (!(err = readdir_r(dd.get(), &entry, &result)) && result != nullptr) {
      if (regex_match(entry.d_name, re)) {
        out.push_back(dir + '/' + entry.d_name);
        ++count;
      }
    }
    if (result != nullptr)
      throw cet::exception(exception_category)
        << "Failed to read directory \"" << dir << "\"; error num = " << err;
  }

  return count;
}

std::string
search_path::to_string() const
{
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

//======================================================================

ostream&
cet::operator<<(ostream& os, search_path const& path)
{
  auto const sz = path.size();
  if (sz == 0)
    return os;
  os << path[0];
  for (size_t k{1}; k != sz; ++k) {
    os << ":" << path[k];
  }
  return os;
}
