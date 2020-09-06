#include <catch2/catch.hpp>

#include "cetlib/exception_category_matcher.h"
#include "cetlib/search_path.h"
#include "cetlib_except/exception.h"

#include <string>

using cet::search_path;

using namespace std::string_literals;

TEST_CASE("Autodetect env")
{
  auto const path = "xyzzy"s;
  search_path const xyzzy{path};
  CHECK(xyzzy.size() == 1);
  CHECK(xyzzy.showenv() == path);
}

TEST_CASE("Autodetect missing env")
{
  auto const path = "/MISSING/"s;
  CHECK_THROWS_MATCHES(search_path{path},
                       cet::exception,
                       cet::exception_category_matcher("getenv"));
}

TEST_CASE("Autodetect path")
{
  search_path const xyzzy{":xyzzy"};
  CHECK(xyzzy.size() == 1);
  CHECK(xyzzy.showenv().empty());
}

TEST_CASE("Missing env OK")
{
  auto const path{"/MISSING/"s};
  search_path const sp{path, std::nothrow};
  CHECK(sp.size() == 1);
  CHECK(sp.showenv() == path);
}

TEST_CASE("Specified path")
{
  search_path const sp{"/onedir", cet::path_tag};
  CHECK(sp.size() == 1);
  CHECK(sp.showenv().empty());
}

TEST_CASE("Path entry count checks")
{
  CHECK(search_path{":xyzzy:"}.size() == 1);
  CHECK(search_path{"xyzzy:plugh"}.size() == 2);
  CHECK(search_path{"xyzzy:::plugh"}.size() == 2);
  CHECK(search_path{"xyzzy:plugh:twisty:grue"}.size() == 4);

  CHECK(!search_path{""}.empty());
  CHECK(!search_path{":"}.empty());
  CHECK(!search_path{"::"}.empty());

  CHECK(search_path{""}.size() == 1);
  CHECK(search_path{":"}.size() == 1);
  CHECK(search_path{"::"}.size() == 1);
}

TEST_CASE("Null find check")
{
  search_path const sp{":/tmp:"};
  CHECK_THROWS_MATCHES(sp.find_file(""),
                       cet::exception,
                       cet::exception_category_matcher("search_path"));
}

TEST_CASE("Path reproduction check")
{
  CHECK(search_path{"a:bb:c.c"}.to_string() == "a:bb:c.c"s);
}
