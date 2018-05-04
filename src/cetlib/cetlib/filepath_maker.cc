// ======================================================================
//
// filepath_maker: A family of related policies governing the translation
//                 of a filename into a fully-qualified filepath
//
// ======================================================================

#include "cetlib/filepath_maker.h"
#include "cetlib/filesystem.h"

#include <iostream>

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
  filepath_first_absolute_or_lookup_with_dot(std::string const& paths)
  : first_paths{std::string("./:") + paths}, after_paths{paths + ':'}
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
  } else {
    return after_paths.find_file(filename);
  }
}

void
filepath_first_absolute_or_lookup_with_dot::reset()
{
  first = true;
}

// ======================================================================
