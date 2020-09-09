// ======================================================================
//
// filepath_maker: A family of related policies governing the translation
//                 of a filename into a fully-qualified filepath
//
// ======================================================================

#include "cetlib/filepath_maker.h"
#include "cetlib/filesystem.h"
#include "cetlib/getenv.h"
#include "cetlib_except/exception.h"

#include <iostream>
#include <map>
#include <vector>

using cet::filepath_first_absolute_or_lookup_with_dot;
using cet::filepath_lookup;
using cet::filepath_lookup_after1;
using cet::filepath_lookup_nonabsolute;
using cet::filepath_maker;

// ----------------------------------------------------------------------

std::string
filepath_maker::operator()(std::string const& filename)
{
  return filename;
}

// ----------------------------------------------------------------------

filepath_lookup::filepath_lookup(std::string paths) : paths{move(paths)} {}

std::string
filepath_lookup::operator()(std::string const& filename)
{
  return paths.find_file(filename);
}

// ----------------------------------------------------------------------

filepath_lookup_nonabsolute::filepath_lookup_nonabsolute(std::string paths)
  : paths{move(paths)}
{}

std::string
filepath_lookup_nonabsolute::operator()(std::string const& filename)
{
  return cet::is_absolute_filepath(filename) ? filename :
                                               paths.find_file(filename);
}

// ----------------------------------------------------------------------

filepath_lookup_after1::filepath_lookup_after1(std::string paths)
  : paths{move(paths)}
{}

std::string
filepath_lookup_after1::operator()(std::string const& filename)
{
  return after1 ? paths.find_file(filename) : (after1 = true, filename);
}

void
filepath_lookup_after1::reset()
{
  after1 = false;
}

// ----------------------------------------------------------------------

filepath_first_absolute_or_lookup_with_dot::
  filepath_first_absolute_or_lookup_with_dot(std::string paths)
  : first_paths{std::string("./:") + paths}, after_paths{move(paths) + ':'}
{
  if (after_paths.empty()) {
    std::cerr << "search path empty (nonexistent environment variable"
              << (paths.empty() ? "" : std::string(" ") + paths) << ")?\n"
              << "Any included configurations will not be found by this lookup "
                 "policy.\n";
  }
}

std::string
filepath_first_absolute_or_lookup_with_dot::operator()(
  std::string const& filename)
{
  if (first) {
    first = false;
    return cet::is_absolute_filepath(filename) ?
             filename :
             first_paths.find_file(filename);
  }
  return after_paths.find_file(filename);
}

void
filepath_first_absolute_or_lookup_with_dot::reset()
{
  first = true;
}

// ======================================================================

std::unique_ptr<filepath_maker>
cet::lookup_policy_selector::select(std::string const& policy,
                                    std::string env_or_paths) const
{
  if (policy == none()) {
    return std::make_unique<filepath_maker>();
  }

  if (policy == all()) {
    return std::make_unique<filepath_lookup>(move(env_or_paths));
  }

  if (policy == nonabsolute()) {
    return std::make_unique<filepath_lookup_nonabsolute>(move(env_or_paths));
  }

  if (policy == after1()) {
    return std::make_unique<filepath_lookup_after1>(move(env_or_paths));
  }

  if (policy == permissive()) {
    auto search_paths = cet::getenv(env_or_paths);
    if (empty(search_paths)) {
      // 'env_or_paths' is not environment variable; assume it's a
      // list of colon-delimited paths.
      search_paths = env_or_paths;
    }
    return std::make_unique<filepath_first_absolute_or_lookup_with_dot>(
      move(search_paths));
  }

  throw cet::exception("Configuration")
    << "An unsupported file-lookup policy was selected.";
}

std::string
cet::lookup_policy_selector::help_message() const
{
  using namespace std::string_literals;
  std::map<std::string, std::vector<std::string>> policies;
  policies.emplace(none(), std::vector{"No lookup is done for any files."s});
  policies.emplace(
    all(),
    std::vector{"Lookup is performed for all files presented to the filepath-"s,
                "making object."s});
  policies.emplace(
    nonabsolute(),
    std::vector{"Lookup is performed only for files that are not absolute--"s,
                "i.e. file names not beginning with the '/' character."s});
  policies.emplace(
    after1(),
    std::vector{"No lookup for the first file, and only lookup is performed"s,
                "for all subsequent files."s});
  policies.emplace(
    permissive(),
    std::vector{
      "The first file can be an absolute path, a path relative to '.',"s,
      "or a path that can be looked up; all subsequent files must be"s,
      "looked up.  This is the policy used by the art framework; it is"s,
      "sometimes referred to as the first-absolute-or-lookup-with-dot"s,
      "policy."s});
  std::string result{"\nThe following file-lookup policies are supported:\n"};
  for (auto const& [name, description] : policies) {
    result += "\n  '";
    result += name;
    result += "'\n";
    for (auto const& line : description) {
      result += "     ";
      result += line;
      result += '\n';
    }
  }
  result += '\n';
  return result;
}
