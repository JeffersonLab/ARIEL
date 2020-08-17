#ifndef _POSIX_C_SOURCE
/* For setenv() */
#define _POSIX_C_SOURCE 200112L
#endif

#include "catch2/catch.hpp"

#include "cetlib/detail/plugin_search_path.h"
#include "cetlib/getenv.h"
#include "cetlib/os_libpath.h"
#include "cetlib/plugin_libpath.h"
#include "cetlib/search_path.h"
#include "cetlib/split_path.h"

#include <cstdlib>
#include <set>
#include <string>

using cet::os_libpath;
using cet::plugin_libpath;
using cet::search_path;
using cet::detail::plugin_search_path;
using std::string;

namespace {
  string const test_path{"/dev/null/nopath:/dev/null/xpath"};

  std::size_t
  path_entries(std::string const& path)
  {
    return (path.empty() ? 0ull :
                           std::count(path.cbegin(), path.cend(), ':') + 1ull);
  }

  std::size_t const tp_sz = path_entries(test_path);

  string
  remove_dups_from_path(string const& path)
  {
    string result;
    std::vector<string> path_bits;
    std::set<string> paths_used;
    cet::split_path(path, path_bits);
    for (auto const& bit : path_bits) {
      if (paths_used.find(bit) == paths_used.end()) {
        if (!result.empty()) {
          result.append(":");
        }
        result.append(bit);
        paths_used.insert(bit);
      }
    }
    return result;
  }
}

TEST_CASE("Tests")
{
  // Setup.
  unsetenv(plugin_libpath());
  string const default_libpath{cet::getenv(os_libpath(), std::nothrow)};
  if (default_libpath.empty()) {
    // Need *something*.
    setenv(os_libpath(), "/dev/null/libpath", 1);
  }
  auto const libpath_before = cet::getenv(os_libpath());

  ////////////////////////////////////
  // Tests.
  ////////////////////////////////////

  // Test using non-empty plugin_libpath().
  SECTION("Standard")
  {
    setenv(plugin_libpath(), test_path.c_str(), 1);
    string const expected{remove_dups_from_path(cet::getenv(plugin_libpath()) +
                                                ":" + libpath_before)};
    auto const sp{plugin_search_path(search_path{plugin_libpath()})};
    CHECK(tp_sz == sp.size());
    auto const libpath = cet::getenv(os_libpath());
    CHECK(expected == libpath);
    CHECK(tp_sz < path_entries(libpath));
  }

  // Test fallback for null plugin_libpath().
  SECTION("Standard Null")
  {
    auto const sp{
      plugin_search_path(search_path{plugin_libpath(), std::nothrow})};
    auto const libpath = cet::getenv(os_libpath());
    CHECK(path_entries(libpath) == sp.size());
    CHECK(remove_dups_from_path(libpath_before) == libpath);
  }

  // Test lack of fallback for empty plugin_libpath().
  SECTION("Standard Empty")
  {
    setenv(plugin_libpath(), "", 1);
    auto const sp{plugin_search_path(search_path{plugin_libpath()})};
    CHECK(1ull == sp.size()); // "."
    CHECK(remove_dups_from_path(libpath_before) == cet::getenv(os_libpath()));
  }

  // Test non-empty arbitrary environment variable.
  SECTION("Custom")
  {
    setenv("SIMPLE_VAR", test_path.c_str(), 1);
    auto const sp{plugin_search_path(search_path{"SIMPLE_VAR"})};
    auto const libpath = cet::getenv(os_libpath());
    CHECK(tp_sz == sp.size());
    string const expected{cet::getenv("SIMPLE_VAR") + ":" + libpath_before};
    CHECK(remove_dups_from_path(expected) == libpath);
    CHECK(tp_sz < path_entries(libpath));
  }

  // Test null arbitrary environment variable.
  SECTION("Custom Null")
  {
    auto const sp{plugin_search_path(search_path{"MISSING_VAR", std::nothrow})};
    CHECK(1ull == sp.size()); // "."
    CHECK(remove_dups_from_path(libpath_before) == cet::getenv(os_libpath()));
  }

  // Test empty arbitrary environment variable.
  SECTION("Custom Empty")
  {
    setenv("EMPTY_VAR", "", 1);
    auto const sp{plugin_search_path(search_path{"EMPTY_VAR"})};
    CHECK(1ull == sp.size()); // "."
    CHECK(remove_dups_from_path(libpath_before) == cet::getenv(os_libpath()));
  }

  // Teardown.
  setenv(os_libpath(), default_libpath.c_str(), 1);
}
