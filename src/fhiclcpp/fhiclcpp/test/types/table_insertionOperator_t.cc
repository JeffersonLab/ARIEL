// ======================================================================
//
// test operator<< for Table
//
// ======================================================================

#define BOOST_TEST_MODULE (table_insertionOperator_t)

#include "cetlib/quiet_unit_test.hpp"
#include "cetlib/test_macros.h"
#include "cetlib_except/demangle.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/OptionalTable.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/detail/optional_parameter_message.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace fhicl;
using namespace std;

namespace {

  struct Config {
    Atom<bool> flag{Name("flag"), true};
    Atom<string> name{Name("name"), "Billy"};
  };

  struct Logger {
    std::string log;

    template <typename T>
    Logger&
    operator<<(T const& t)
    {
      std::ostringstream oss;
      oss << t;
      log.append(oss.str());
      return *this;
    }
  };

  std::string
  reference()
  {
    return "\n   " + detail::optional_parameter_message() +
           "\n\n"
           "   config: {\n"
           "\n"
           "      flag: true  # default\n"
           "\n"
           "      name: \"Billy\"  # default\n"
           "   }\n";
  }

  std::string
  optional_reference()
  {
    return "\n   " + detail::optional_parameter_message() +
           "\n\n"
           " # config: {\n"
           " #\n"
           " #    flag: true  # default\n"
           " #\n"
           " #    name: \"Billy\"  # default\n"
           " # }\n";
  }
}

BOOST_AUTO_TEST_SUITE(table_insertionOperator_test)

BOOST_AUTO_TEST_CASE(simple)
{
  Table<Config> config{Name("config")};
  ostringstream oss;
  oss << config;
  auto const& test = oss.str();
  auto const& ref = reference();
  BOOST_CHECK_EQUAL(test, ref);
}

BOOST_AUTO_TEST_CASE(logger)
{
  Table<Config> config{Name("config")};
  Logger logger;
  logger << config;
  BOOST_CHECK_EQUAL(logger.log, reference());

  Logger const logger2 = (Logger() << config);
  BOOST_CHECK_EQUAL(logger2.log, reference());
}

BOOST_AUTO_TEST_CASE(loggerOptional)
{
  OptionalTable<Config> config{Name("config")};
  Logger logger;
  logger << config;
  BOOST_CHECK_EQUAL(logger.log, optional_reference());

  Logger const logger2 = (Logger() << config);
  BOOST_CHECK_EQUAL(logger2.log, optional_reference());
}

BOOST_AUTO_TEST_SUITE_END()
