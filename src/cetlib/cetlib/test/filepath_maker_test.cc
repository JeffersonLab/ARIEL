// ======================================================================
//
// filepath_maker_test
//
// ======================================================================

#define BOOST_TEST_MODULE (filepath_maker test)
#include "cetlib/quiet_unit_test.hpp"

#include "boost/filesystem.hpp"
#include "cetlib/filepath_maker.h"
#include "cetlib/filesystem.h"
#include "cetlib/getenv.h"
#include "cetlib_except/exception.h"

#include <iostream>
#include <string>

namespace bfs = boost::filesystem;
using namespace std::string_literals;

namespace {
  std::string const path{"FILEPATH_MAKER_TEST_FILES"};
  std::string const file_in_current_dir{"filepath_maker_test.txt"};

  inline std::string
  current_dir()
  {
    return cet::getenv("CURRENT_DIR");
  }

  inline std::string
  current_nested_dir()
  {
    return current_dir() + "/filepath_maker-files";
  }

  void
  check_exception(cet::filepath_maker& maker, std::string const& filepath)
  {
    BOOST_CHECK_EXCEPTION(
      maker(filepath), cet::exception, [](cet::exception const& e) {
        return e.category() == "search_path";
      });
  }
}

BOOST_AUTO_TEST_SUITE(filepath_maker_test)

// operator() does not translate the input to a full path at all
BOOST_AUTO_TEST_CASE(filepath_maker_t)
{
  cet::filepath_maker maker{};
  auto const files = {"a.txt", "./b.txt", "/c/d.txt"};
  for (auto const& filename : files) {
    BOOST_CHECK_EQUAL(filename, maker(filename));
  }
}

BOOST_AUTO_TEST_CASE(filepath_lookup_t1)
{
  // Check that
  cet::filepath_lookup maker{path};
  auto const files = {"a.txt"s, "b.txt"s, "c.txt"s};
  for (auto const& filename : files) {
    auto const fullPath1 = maker(filename);
    // Adding './' or '/' makes no difference when everything is
    // looked up relative to the specified paths.
    auto const fullPath2 = maker("./" + filename);
    auto const fullPath3 = maker("/" + filename);
    bfs::path const p1{fullPath1};
    bfs::path const p2{fullPath2};
    bfs::path const p3{fullPath3};
    BOOST_CHECK(bfs::equivalent(p1, p2));
    BOOST_CHECK(bfs::equivalent(p1, p3));
    // Check that operator() has created an absolute filepath.
    BOOST_CHECK(cet::is_absolute_filepath(fullPath1));
    BOOST_CHECK(cet::is_absolute_filepath(fullPath2));
    BOOST_CHECK(cet::is_absolute_filepath(fullPath3));
    // Now check that the file *cannot* be accessed via its absolute filepath.
    check_exception(maker, current_nested_dir() + '/' + filename);
  }
}

BOOST_AUTO_TEST_CASE(filepath_lookup_t2)
{
  cet::filepath_lookup maker{path};
  // Check that we cannot access a file in the current source
  // directory.
  check_exception(maker, file_in_current_dir);
}

BOOST_AUTO_TEST_CASE(filepath_lookup_nonabsolute_t)
{
  cet::filepath_lookup_nonabsolute maker{path};
  auto const files = {"a.txt"s, "b.txt"s, "c.txt"s};
  for (auto const& filename : files) {
    auto const fullPath1 = maker(filename);
    // Test that it can be accessed via the absolute path
    auto const fullPath2 = maker(current_nested_dir() + '/' + filename);
    bfs::path const p1{fullPath1};
    bfs::path const p2{fullPath2};
    BOOST_CHECK(bfs::equivalent(p1, p2));
    // Check that operator() has created an absolute filepath.
    BOOST_CHECK(cet::is_absolute_filepath(fullPath1));
    BOOST_CHECK(cet::is_absolute_filepath(fullPath2));
  }

  // Check that we cannot access a file in the current source
  // directory without specifying the full path...
  check_exception(maker, file_in_current_dir);
  // ...but we can access it through its full path.
  maker(current_dir() + '/' + file_in_current_dir);
}

BOOST_AUTO_TEST_CASE(filepath_lookup_after1_t1)
{
  cet::filepath_lookup_after1 maker{path};
  // For the first file, the maker returns whatever you give it.
  // There is no checking to see if the file actually exists.
  auto const file = maker("a.txt");
  BOOST_CHECK(!cet::file_exists(file));
}

BOOST_AUTO_TEST_CASE(filepath_lookup_after1_t2)
{
  cet::filepath_lookup_after1 maker{path};
  auto const absolute_path = current_dir() + '/' + file_in_current_dir;
  // Absolute allowed for first file
  maker(absolute_path);
  // ... not allowed for second file
  check_exception(maker, absolute_path);

  // File relative to path allowed for second file
  maker("a.txt");
  // Absolute path not allowed for second file
  check_exception(maker, current_nested_dir() + "/a.txt");
}

BOOST_AUTO_TEST_CASE(filepath_lookup_after1_t3)
{
  cet::filepath_lookup_after1 maker{path};
  auto const relative_path = "./" + file_in_current_dir;
  // Dot-prefixed file allowed for first file
  maker(relative_path);
  // ... not allowed for second file
  check_exception(maker, relative_path);

  // File relative to path allowed for second file
  maker("a.txt");
  // Absolute path not allowed for second file
  check_exception(maker, current_nested_dir() + "/a.txt");
}

BOOST_AUTO_TEST_CASE(filepath_lookup_after1_t4)
{
  cet::filepath_lookup_after1 maker{path};
  // Relative file allowed for first file
  maker(file_in_current_dir);
  // ... not allowed for second file
  check_exception(maker, file_in_current_dir);

  // File relative to path allowed for second file
  maker("a.txt");
  // Absolute path not allowed for second file
  check_exception(maker, current_nested_dir() + "/a.txt");
}

BOOST_AUTO_TEST_CASE(filepath_first_absolute_or_lookup_with_dot_t1)
{
  // Must provide value of environment variable for this policy, not
  // its name.
  cet::filepath_first_absolute_or_lookup_with_dot maker{cet::getenv(path)};
  // The first file is permitted to be relative the path.
  auto const file = maker("a.txt");
  BOOST_CHECK(cet::file_exists(file));
}

BOOST_AUTO_TEST_CASE(filepath_first_absolute_or_lookup_with_dot_t2)
{
  cet::filepath_first_absolute_or_lookup_with_dot maker{cet::getenv(path)};
  auto const absolute_path = current_dir() + '/' + file_in_current_dir;
  // Absolute allowed for first file
  maker(absolute_path);
  // ... not allowed for second file
  check_exception(maker, absolute_path);

  // File relative to path allowed for second file
  maker("a.txt");
  // Absolute path not allowed for second file
  check_exception(maker, current_nested_dir() + "/a.txt");
}

BOOST_AUTO_TEST_CASE(filepath_first_absolute_or_lookup_with_dot_t3)
{
  cet::filepath_first_absolute_or_lookup_with_dot maker{cet::getenv(path)};
  auto const relative_path = "./" + file_in_current_dir;
  // Dot-prefixed file allowed for first file
  maker(relative_path);
  // ... not allowed for second file
  check_exception(maker, relative_path);

  // File relative to path allowed for second file
  maker("a.txt");
  // Absolute path not allowed for second file
  check_exception(maker, current_nested_dir() + "/a.txt");
}

BOOST_AUTO_TEST_CASE(filepath_first_absolute_or_lookup_with_dot_t4)
{
  cet::filepath_first_absolute_or_lookup_with_dot maker{cet::getenv(path)};
  // Relative file allowed for first file
  maker(file_in_current_dir);
  // ... not allowed for second file
  check_exception(maker, file_in_current_dir);

  // File relative to path allowed for second file
  maker("a.txt");
  // Absolute path not allowed for second file
  check_exception(maker, current_nested_dir() + "/a.txt");
}

BOOST_AUTO_TEST_SUITE_END()
