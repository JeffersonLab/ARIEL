#define BOOST_TEST_MODULE (is_absolute_filepath_t test)
#include "cetlib/quiet_unit_test.hpp"

#include <string>

#include "cetlib/filesystem.h"

BOOST_AUTO_TEST_SUITE(is_absolute_filepath_t)

BOOST_AUTO_TEST_CASE(accept_absolute_filenames)
{
  std::string const absolute1("/a/b/c.txt");
  BOOST_TEST(cet::is_absolute_filepath(absolute1));
  BOOST_TEST(cet::is_absolute_filepath("/a/b/ccc"));
  BOOST_TEST(cet::is_absolute_filepath("/"));

  BOOST_TEST(!cet::is_relative_filepath(absolute1));
  BOOST_TEST(!cet::is_relative_filepath("/a/b/ccc"));
  BOOST_TEST(!cet::is_relative_filepath("/"));
}

BOOST_AUTO_TEST_CASE(reject_relative_filenames)
{
  std::string const relative("a/b/c.txt");
  BOOST_TEST(!cet::is_absolute_filepath(relative));
  BOOST_TEST(!cet::is_absolute_filepath(""));
  BOOST_TEST(!cet::is_absolute_filepath("."));
  BOOST_TEST(!cet::is_absolute_filepath("../"));

  BOOST_TEST(cet::is_relative_filepath(relative));
  BOOST_TEST(cet::is_relative_filepath(""));
  BOOST_TEST(cet::is_relative_filepath("."));
  BOOST_TEST(cet::is_relative_filepath("../"));
}

BOOST_AUTO_TEST_SUITE_END()
