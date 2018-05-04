#define BOOST_TEST_MODULE (search_path_test_3 test)
#include "cetlib/quiet_unit_test.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include "cetlib/include.h"
#include "cetlib_except/exception.h"

BOOST_AUTO_TEST_SUITE(search_path_test_3)

BOOST_AUTO_TEST_CASE(missing_file_throws)
{
  std::istringstream is("#include \"/tmp/aaa\"");
  std::string result;

  BOOST_CHECK_THROW(cet::include(is, result), cet::exception);
}

BOOST_AUTO_TEST_CASE(bad_stream_throws)
{
  std::ifstream is("no file with this name exists");
  std::string result;

  BOOST_CHECK_THROW(cet::include(is, result), cet::exception);
}

BOOST_AUTO_TEST_SUITE_END()
