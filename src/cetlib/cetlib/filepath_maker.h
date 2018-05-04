#ifndef cetlib_filepath_maker_h
#define cetlib_filepath_maker_h

// ======================================================================
//
// filepath_maker: A family of related policies governing the translation
//                 of a filename into a fully-qualified filepath
//
// The semantics of each of the policies are as follows:
//
// filepath_maker: No lookup at all.
//
// filepath_lookup: Only lookup permitted.

// filepath_lookup_nonabsolute: Absolute paths are allowed, but lookup
//   occurs for non-absolute paths.
//
// filepath_lookup_after1: No lookup for the first file, and only
//   lookup for all subsequent files.
//
// filepath_first_absolute_or_lookup_with_dot: The first file can be
//   an absolute path, a path relative to '.', or a path that can be
//   looked up; all subsequent files must be looked up.
//
// ======================================================================

#include "cetlib/search_path.h"
#include <string>

namespace cet {
  class filepath_maker;
  class filepath_lookup;
  class filepath_lookup_nonabsolute;
  class filepath_lookup_after1;
  class filepath_first_absolute_or_lookup_with_dot;
}

// ----------------------------------------------------------------------

class cet::filepath_maker {
public:
  virtual std::string operator()(std::string const& filename);

  virtual ~filepath_maker() noexcept = default;

}; // filepath_maker

// ----------------------------------------------------------------------

class cet::filepath_lookup : public cet::filepath_maker {
public:
  filepath_lookup(std::string paths);

  std::string operator()(std::string const& filename) override;

private:
  cet::search_path paths;

}; // filepath_lookup

// ----------------------------------------------------------------------

class cet::filepath_lookup_nonabsolute : public cet::filepath_maker {
public:
  filepath_lookup_nonabsolute(std::string paths);

  std::string operator()(std::string const& filename) override;

private:
  cet::search_path paths;

}; // filepath_lookup_nonabsolute

// ----------------------------------------------------------------------

class cet::filepath_lookup_after1 : public cet::filepath_maker {
public:
  filepath_lookup_after1(std::string paths);

  std::string operator()(std::string const& filename) override;

  void reset();

private:
  bool after1{false};
  cet::search_path paths;

}; // filepath_lookup_after1

// ----------------------------------------------------------------------

class cet::filepath_first_absolute_or_lookup_with_dot
  : public cet::filepath_maker {
public:
  // The provided string must be the *value* of the environment
  // variable, *not* its name.
  filepath_first_absolute_or_lookup_with_dot(std::string const& paths);
  std::string operator()(std::string const& filename) override;
  void reset();

private:
  bool first{true};
  cet::search_path first_paths;
  cet::search_path after_paths;
}; // filepath_first_absolute_or_lookup_with_dot

// ======================================================================

#endif /* cetlib_filepath_maker_h */

// Local Variables:
// mode: c++
// End:
