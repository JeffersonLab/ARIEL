#define BOOST_TEST_MODULE (intermediate_table_t)
#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace fhicl;

BOOST_AUTO_TEST_SUITE(intermediate_table_t)

BOOST_AUTO_TEST_CASE(main)
{
  intermediate_table table;
  using table_t = intermediate_table::table_t;
  using sequence_t = intermediate_table::sequence_t;
  table.put("string1", "string");
  table.put("string2", std::string("string"));
  table.put("complex", std::complex<double>(-3, -0.5));
  table.put("sequence", std::vector<int>{5, 10, 15, 20, 25});
  table.put("double", 28.5);
  table.put("sequence[6]", 35);
  table.put("table.val1", 92);
  table.put("table.t1.val", "oops");
  BOOST_CHECK_NO_THROW(table.erase("table.t1.waldo"));
  BOOST_CHECK_NO_THROW(table.erase("table.t1.val"));
  BOOST_TEST(!table.exists("table.t1.val"));
  BOOST_CHECK_THROW(table.erase("table.val1.valX"), fhicl::exception);
  BOOST_TEST(table.get<std::string>("string1") == "string");
  BOOST_TEST(table.get<std::string>("string2") == "string");
  BOOST_TEST(table.get<std::complex<double>>("complex") ==
             std::complex<double>(-3, -0.5));
  BOOST_TEST(table.get<sequence_t const&>("sequence").size() == 7u);
  BOOST_TEST(table.get<int>("sequence[3]") == 20);
  BOOST_CHECK_THROW(table.get<int>("sequence[5]"), fhicl::exception); // Nil
  BOOST_TEST(table.get<table_t const&>("table").size() == 2u);
  BOOST_TEST(table.get<table_t const&>("table.t1").size() == 0u);
  ParameterSet pset;
  make_ParameterSet(table, pset);
}

BOOST_AUTO_TEST_CASE(prolog_erase_nested)
{
  char const* cfg = R"(BEGIN_PROLOG
w: { x: { y: { z: { a: 2 b: 3 } } } }
END_PROLOG
q: { @table::w.x.y })";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("q.z"));
  BOOST_TEST(!pset.has_key("w"));
}

BOOST_AUTO_TEST_CASE(prolog_erase_dotted)
{
  char const* cfg = R"(BEGIN_PROLOG
w.x.y: { z: { a: 2 b: 3 } }
END_PROLOG
q: { @table::w.x.y })";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("q.z"));
  BOOST_TEST(!pset.has_key("w"));
}

BOOST_AUTO_TEST_CASE(prolog_nested_partial_dup_nested)
{
  char const* cfg = R"(BEGIN_PROLOG
w: { x: { y: 7 } }
END_PROLOG
w: { b: { c: 6 } })";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("w.b"));
  BOOST_TEST(!pset.has_key("w.x"));
}

BOOST_AUTO_TEST_CASE(prolog_nested_partial_dup_dotted)
{
  char const* cfg = R"(BEGIN_PROLOG
w: { x: { y: 7 } }
END_PROLOG
w.b.c: 6)";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("w.b"));
  BOOST_TEST(!pset.has_key("w.x"));
}

BOOST_AUTO_TEST_CASE(prolog_dotted_partial_dup_nested)
{
  char const* cfg = R"(BEGIN_PROLOG
w.x.y: 7
END_PROLOG
w: { b: { c: 6 } })";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("w.b"));
  BOOST_TEST(!pset.has_key("w.x"));
}

BOOST_AUTO_TEST_CASE(prolog_dotted_partial_dup_dotted)
{
  char const* cfg = R"(BEGIN_PROLOG
w.x.y: 7
END_PROLOG
w.b.c: 6)";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("w.b"));
  BOOST_TEST(!pset.has_key("w.x"));
}

BOOST_AUTO_TEST_CASE(prolog_nested_dup_nested)
{
  char const* cfg = R"(BEGIN_PROLOG
w: { x: { y: 7 } }
END_PROLOG
w: { x: { c: 6 } })";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("w.x.c"));
  BOOST_TEST(!pset.has_key("w.x.y"));
}

BOOST_AUTO_TEST_CASE(prolog_nested_dup_dotted)
{
  char const* cfg = R"(BEGIN_PROLOG
w: { x: { y: 7 } }
END_PROLOG
w.x.c: 6)";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("w.x.c"));
  BOOST_TEST(!pset.has_key("w.x.y"));
}

BOOST_AUTO_TEST_CASE(prolog_dotted_dup_nested)
{
  char const* cfg = R"(BEGIN_PROLOG
w.x.y: 7
END_PROLOG
w: { x: { c: 6 } })";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("w.x.c"));
  BOOST_TEST(!pset.has_key("w.x.y"));
}

BOOST_AUTO_TEST_CASE(prolog_dotted_dup_dotted)
{
  char const* cfg = R"(BEGIN_PROLOG
w.x.y: 7
END_PROLOG
w.x.c: 6)";
  ParameterSet pset;
  make_ParameterSet(cfg, pset);
  BOOST_TEST(pset.has_key("w.x.c"));
  BOOST_TEST(!pset.has_key("w.x.y"));
}

BOOST_AUTO_TEST_SUITE_END()
