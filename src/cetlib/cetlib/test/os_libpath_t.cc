#include "catch2/catch.hpp"

#include "cetlib/os_libpath.h"

TEST_CASE("Basic test")
{
  std::string const expected =
#ifdef __APPLE__
    "DYLD_LIBRARY_PATH";
#else
    "LD_LIBRARY_PATH";
#endif

  char const* lpath = cet::os_libpath();
  CHECK(expected == lpath);
}
