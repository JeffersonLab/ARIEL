#include "cetlib/LibraryManager.h"

#define BOOST_TEST_MODULE (LibraryManager Test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/container_algorithms.h"
#include "cetlib/test/LibraryManagerTestFunc.h"
#include "cetlib_except/exception.h"

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace cet;

// LibraryManager tests are independent of how it's constructed so
// make test fixture creation compile time generated.  This allows >1
// test to be built.
#if defined(LIBRARY_MANAGER_SEARCH_PATH)
// Construction using search_path
struct LibraryManagerTestFixture {

  LibraryManagerTestFixture();

  LibraryManager lm;
  LibraryManager const& lm_ref;
};

LibraryManagerTestFixture::LibraryManagerTestFixture()
  : lm{search_path{"LIBRARY_MANAGER_SEARCH_PATH"}, "cetlibtest"}, lm_ref{lm}
{}
#else
// Default construction, should use system dynamic loader path
struct LibraryManagerTestFixture {

  LibraryManagerTestFixture();

  LibraryManager lm;
  LibraryManager const& lm_ref;
};

LibraryManagerTestFixture::LibraryManagerTestFixture()
  : lm{"cetlibtest"}, lm_ref{lm}
{}
#endif

BOOST_FIXTURE_TEST_SUITE(LibraryManagerTests, LibraryManagerTestFixture)

BOOST_AUTO_TEST_CASE(libSpecsVector)
{
  std::vector<std::string> lib_list;
  BOOST_TEST_REQUIRE(lm_ref.getValidLibspecs(lib_list) > 0);
}

BOOST_AUTO_TEST_CASE(libSpecsIter)
{
  std::vector<std::string> lib_list;
  BOOST_TEST_REQUIRE(lm_ref.getValidLibspecs(std::back_inserter(lib_list)) > 0);
}

BOOST_AUTO_TEST_CASE(libListVector)
{
  std::vector<std::string> lib_list;
  BOOST_TEST_REQUIRE(lm_ref.getLoadableLibraries(lib_list) > 0);
}

BOOST_AUTO_TEST_CASE(libListIter)
{
  std::vector<std::string> lib_list;
  BOOST_TEST_REQUIRE(lm_ref.getLoadableLibraries(std::back_inserter(lib_list)) >
                     0);
}

BOOST_AUTO_TEST_CASE(getSymbolLong)
{
  BOOST_TEST_REQUIRE(lm_ref.getSymbolByLibspec<void*>("2/1/5", "idString") !=
                     nullptr);
}

BOOST_AUTO_TEST_CASE(getSymbolShort)
{
  BOOST_TEST_REQUIRE(lm_ref.getSymbolByLibspec<void*>("5", "idString") !=
                     nullptr);
}

BOOST_AUTO_TEST_CASE(getSymbolPathPrecedence)
{
  BOOST_CHECK_NO_THROW(lm_ref.getSymbolByLibspec<void*>("1/1/1", "idString"));
}

BOOST_AUTO_TEST_CASE(getSymbolAmbiguity)
{
  BOOST_CHECK_EXCEPTION(
    lm_ref.getSymbolByLibspec<void*>("3", "idString"),
    cet::exception,
    [](cet::exception const& e) { return e.category() == "Configuration"; });
}

namespace {
  void
  verify(std::string libspec, cettest::idString_t idString)
  {
    std::size_t pos{};
    while ((pos = libspec.find_first_of('/', pos)) != std::string::npos) {
      libspec[pos] = '_';
    }
    BOOST_TEST_REQUIRE(libspec == idString());
  }
}

BOOST_AUTO_TEST_CASE(getSymbolNoAmbiguity1)
{
  std::string const libspecA{"1/2/3"};
  std::string const libspecB{"1/1/3"};
  cettest::idString_t idString{nullptr};
  BOOST_CHECK_NO_THROW(
    idString =
      lm_ref.getSymbolByLibspec<cettest::idString_t>(libspecA, "idString"));
  verify(libspecA, idString);
  BOOST_CHECK_NO_THROW(
    idString =
      lm_ref.getSymbolByLibspec<cettest::idString_t>(libspecB, "idString"));
  verify(libspecB, idString);
  BOOST_CHECK_NO_THROW(
    idString =
      lm_ref.getSymbolByLibspec<cettest::idString_t>(libspecA, "idString"));
  verify(libspecA, idString);
}

BOOST_AUTO_TEST_CASE(getSymbolNoAmbiguity2)
{
  std::string const libspecA{"1/1/3"};
  std::string const libspecB{"1/2/3"};
  cettest::idString_t idString{nullptr};
  BOOST_CHECK_NO_THROW(
    idString =
      lm_ref.getSymbolByLibspec<cettest::idString_t>(libspecA, "idString"));
  verify(libspecA, idString);
  BOOST_CHECK_NO_THROW(
    idString =
      lm_ref.getSymbolByLibspec<cettest::idString_t>(libspecB, "idString"));
  verify(libspecB, idString);
  BOOST_CHECK_NO_THROW(
    idString =
      lm_ref.getSymbolByLibspec<cettest::idString_t>(libspecA, "idString"));
  verify(libspecA, idString);
}

BOOST_AUTO_TEST_CASE(dictLoadable)
{
  std::vector<std::string> lib_list;
  lm_ref.getLoadableLibraries(lib_list);
  BOOST_TEST_REQUIRE(lm_ref.libraryIsLoadable(*lib_list.begin()));
}

BOOST_AUTO_TEST_CASE(dictLoadableButNotLoaded)
{
  std::vector<std::string> lib_list;
  lm_ref.getLoadableLibraries(lib_list);
  BOOST_TEST_REQUIRE(lm_ref.libraryIsLoadable(*lib_list.begin()));
  BOOST_TEST_REQUIRE(!lm_ref.libraryIsLoaded(*lib_list.begin()));
}

BOOST_AUTO_TEST_CASE(loadAllLibraries)
{
  BOOST_REQUIRE_NO_THROW(lm_ref.loadAllLibraries());
}

BOOST_AUTO_TEST_CASE(dictLoaded)
{
  std::vector<std::string> lib_list;
  lm_ref.getLoadableLibraries(lib_list);
  lm_ref.loadAllLibraries();
  BOOST_TEST_REQUIRE(lm_ref.libraryIsLoaded(*lib_list.begin()));
}

BOOST_AUTO_TEST_CASE(dictNotLoadable)
{
  std::vector<std::string> lib_list;
  lm_ref.getLoadableLibraries(lib_list);
  BOOST_TEST_REQUIRE(!lm_ref.libraryIsLoadable("UnknownLibrary"));
}

BOOST_AUTO_TEST_SUITE_END()
