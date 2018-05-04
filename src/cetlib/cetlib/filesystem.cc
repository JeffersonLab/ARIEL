// ======================================================================
//
// filesystem: A family of file-level utilities
//
// ======================================================================

#include "cetlib/filesystem.h"

#include <sys/stat.h>

// ----------------------------------------------------------------------

bool
cet::file_exists(std::string const& qualified_filename)
{
  struct stat file_info;
  return 0 == stat(qualified_filename.c_str(), &file_info);
}

// ----------------------------------------------------------------------

bool
cet::is_absolute_filepath(std::string const& qualified_filename)
{
  return qualified_filename[0] == '/';
}

bool
cet::is_relative_filepath(std::string const& qualified_filename)
{
  return !is_absolute_filepath(qualified_filename);
}

// ======================================================================
