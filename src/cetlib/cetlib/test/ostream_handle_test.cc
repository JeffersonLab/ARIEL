#define BOOST_TEST_MODULE (ostream_handle test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/exempt_ptr.h"
#include "cetlib/ostream_handle.h"

#include <fstream>
#include <iostream>
#include <string>

using cet::ostream_handle;
using namespace std::string_literals;

namespace {
  std::string
  file_contents(std::string const& fileName)
  {
    std::string result;
    std::ifstream file{fileName};
    for (std::string l{}; std::getline(file, l); result += l) {
    }
    return result;
  }
}

BOOST_AUTO_TEST_SUITE(ostream_handle_test)

BOOST_AUTO_TEST_CASE(observer)
{
  std::ostringstream oss;
  ostream_handle obs{oss};
  obs << "a";
  BOOST_TEST(oss.str() == "a"s);
}

BOOST_AUTO_TEST_CASE(owner)
{
  std::string const fileName{"owner.txt"};
  {
    ostream_handle own{fileName};
    BOOST_TEST(static_cast<bool>(own));
    own << "a";
  }
  BOOST_TEST(file_contents(fileName) == "a"s);
}

BOOST_AUTO_TEST_CASE(observerThenOwner)
{
  std::string const fileName{"observerThenOwner.txt"};
  {
    std::ostringstream oss;
    ostream_handle os{oss};
    os << "a";
    ostream_handle own{fileName};
    own << "b";
    os = std::move(own);
    os << "c";
  }
  BOOST_TEST(file_contents(fileName) == "bc"s);
}

BOOST_AUTO_TEST_CASE(ownerThenObserver)
{
  std::string const fileName{"ownerThenObserver.txt"};
  {
    ostream_handle os{fileName};
    os << "a";
    std::ostringstream oss;
    ostream_handle obs{oss};
    obs << "b";
    os = std::move(obs);
    os << "c";
    BOOST_TEST(oss.str() == "bc"s);
  }
  BOOST_TEST(file_contents(fileName) == "a"s);
}

BOOST_AUTO_TEST_SUITE_END()
