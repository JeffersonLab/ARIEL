#define BOOST_TEST_MODULE (PluginFactory_t)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/BasicPluginFactory.h"
#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/test/TestPluginBase.h"
#include "cetlib_except/exception.h"

#include <memory>
#include <string>

using namespace cet;

// PluginFactory tests are independent of how its search path is
// constructed.

// Make test fixture creation compile time generated so we can
// generated one test for the system default, and one for a
// user-supplied search path.
#if defined(PLUGIN_FACTORY_SEARCH_PATH)
struct PluginFactoryTestFixture {
  explicit PluginFactoryTestFixture() { pf.setDiagReleaseVersion("ETERNAL"); }
  BasicPluginFactory pf{search_path{"PLUGIN_FACTORY_SEARCH_PATH"}};
};
#else
struct PluginFactoryTestFixture {
  explicit PluginFactoryTestFixture() { pf.setDiagReleaseVersion("ETERNAL"); }
  BasicPluginFactory pf{};
};
#endif

using namespace std::string_literals;

BOOST_FIXTURE_TEST_SUITE(PluginFactory_t, PluginFactoryTestFixture)

BOOST_AUTO_TEST_CASE(checkType)
{
  BOOST_REQUIRE_EQUAL("TestPluginBase"s,
                      PluginTypeDeducer<cettest::TestPluginBase>::value);
  BOOST_REQUIRE_EQUAL(pf.pluginType("TestPlugin"),
                      PluginTypeDeducer<cettest::TestPluginBase>::value);
}

BOOST_AUTO_TEST_CASE(checkMaker)
{
  auto p = pf.makePlugin<std::unique_ptr<cettest::TestPluginBase>, std::string>(
    "TestPlugin", "Hi");
  BOOST_REQUIRE_EQUAL(p->message(), "Hi"s);
}

BOOST_AUTO_TEST_CASE(CheckFinder)
{
  auto fptr = pf.find<std::string>(
    "TestPlugin", "pluginType", cet::PluginFactory::nothrow);
  BOOST_REQUIRE(fptr);
  BOOST_REQUIRE_EQUAL(fptr(),
                      PluginTypeDeducer<cettest::TestPluginBase>::value);
  BOOST_REQUIRE(pf.find<std::string>("TestPlugin",
                                     "oops",
                                     cet::PluginFactory::nothrow) == nullptr);
}

BOOST_AUTO_TEST_CASE(checkError)
{
  BOOST_CHECK_EXCEPTION(pf.makePlugin<std::unique_ptr<cettest::TestPluginBase>>(
                          "TestPluginX"s, "Hi"s),
                        cet::exception,
                        [](cet::exception const& e) {
                          return e.category() == "Configuration" &&
                                 std::string{e.what()}.find("ETERNAL") !=
                                   std::string::npos;
                        });
}

BOOST_AUTO_TEST_SUITE_END()
