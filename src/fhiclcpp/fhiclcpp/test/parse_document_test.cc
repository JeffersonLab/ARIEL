// ======================================================================
//
// test ParameterSet values
//
// ======================================================================

#define BOOST_TEST_MODULE (document test)

#include "cetlib/quiet_unit_test.hpp"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"
#include <iostream>
#include <regex>
#include <string>
using namespace fhicl;
using namespace std;

BOOST_AUTO_TEST_SUITE(document_test)

BOOST_AUTO_TEST_CASE(empty_document)
{
  std::string document;
  intermediate_table tbl;
  BOOST_REQUIRE_NO_THROW(parse_document(document, tbl));
  ParameterSet pset;
  make_ParameterSet(tbl, pset);
  BOOST_CHECK(pset.is_empty());
}

BOOST_AUTO_TEST_CASE(nonempty_document)
{
  std::string document = "a : 1\n"
                         "b : 2\n";
  intermediate_table tbl;
  BOOST_REQUIRE_NO_THROW(parse_document(document, tbl));
  ParameterSet pset;
  make_ParameterSet(tbl, pset);
  BOOST_CHECK(!pset.is_empty());
  BOOST_CHECK_EQUAL(pset.get<int>("a"), 1);
  BOOST_CHECK_EQUAL(pset.get<int>("b"), 2);
  BOOST_CHECK_EQUAL(pset.get<unsigned>("a"), 1u);
  BOOST_CHECK_EQUAL(pset.get<unsigned>("b"), 2u);
  BOOST_CHECK_EQUAL(pset.get<string>("a"), "1");
  BOOST_CHECK_EQUAL(pset.get<string>("b"), "2");
}

BOOST_AUTO_TEST_CASE(nested_document)
{
  std::string document = "x.a : 1\n"
                         "x.b : 2\n";
  intermediate_table tbl;
  BOOST_REQUIRE_NO_THROW(parse_document(document, tbl));
  ParameterSet pset;
  make_ParameterSet(tbl, pset);
  BOOST_CHECK(!pset.is_empty());
  BOOST_REQUIRE_NO_THROW(pset.get<ParameterSet>("x"));
  BOOST_REQUIRE(!pset.get<ParameterSet>("x").is_empty());
  BOOST_CHECK_EQUAL(pset.get<int>("x.a"), 1);
  BOOST_CHECK_EQUAL(pset.get<int>("x.b"), 2);
  BOOST_CHECK_EQUAL(pset.get<unsigned>("x.a"), 1u);
  BOOST_CHECK_EQUAL(pset.get<unsigned>("x.b"), 2u);
  BOOST_CHECK_EQUAL(pset.get<string>("x.a"), "1");
  BOOST_CHECK_EQUAL(pset.get<string>("x.b"), "2");
}

BOOST_AUTO_TEST_CASE(badly_nested_document)
{
  std::string document = "{ x.a : 1\n"
                         "  x.b : 2\n"
                         "}\n";
  intermediate_table tbl;
  BOOST_CHECK_THROW(parse_document(document, tbl), cet::exception);
}

BOOST_AUTO_TEST_CASE(overridden_prolog_document)
{
  std::string document = "BEGIN_PROLOG\n"
                         "  a: 1\n"
                         "  t: { a : 11\n"
                         "       b : 12\n"
                         "     }\n"
                         "END_PROLOG\n"
                         "a   : 2\n"
                         "t.a : @local::t.b\n";
  intermediate_table tbl;
  BOOST_REQUIRE_NO_THROW(parse_document(document, tbl));
  ParameterSet pset;
  make_ParameterSet(tbl, pset);
  BOOST_CHECK_EQUAL(pset.get<int>("a"), 2);
  BOOST_REQUIRE_NO_THROW(pset.get<ParameterSet>("t"));
  BOOST_REQUIRE(!pset.get<ParameterSet>("t").is_empty());
  BOOST_CHECK_EQUAL(pset.get<int>("t.a"), 12);
  BOOST_CHECK_THROW(pset.get<int>("t.b"), cet::exception);
}

BOOST_AUTO_TEST_CASE(contiguous_prolog)
{
  std::string document = "BEGIN_PROLOG\n"
                         "  a: 1\n"
                         "  t: { a : 11\n"
                         "       b : 12\n"
                         "     }\n"
                         "END_PROLOG\n"
                         "BEGIN_PROLOG\n"
                         " c: 47\n"
                         "END_PROLOG\n";
  intermediate_table tbl;
  BOOST_REQUIRE_NO_THROW(parse_document(document, tbl));
  BOOST_CHECK(tbl.exists("c"));
}

BOOST_AUTO_TEST_CASE(noncontiguous_prolog)
{
  std::string document = "BEGIN_PROLOG\n"
                         "  a: 1\n"
                         "  t: { a : 11\n"
                         "       b : 12\n"
                         "     }\n"
                         "END_PROLOG\n"
                         "d: 27\n"
                         "BEGIN_PROLOG\n"
                         " c: 47\n"
                         "END_PROLOG\n";
  intermediate_table tbl;
  BOOST_REQUIRE_THROW(parse_document(document, tbl), cet::exception);
}

BOOST_AUTO_TEST_CASE(overridden_toplevel_document)
{
  std::string document = "a : 1\n"
                         "b : 2\n"
                         "a : 3\n"
                         "c : 4\n"
                         "b : 5\n"
                         "a : 6\n";
  intermediate_table tbl;
  BOOST_REQUIRE_NO_THROW(parse_document(document, tbl));
  ParameterSet pset;
  make_ParameterSet(tbl, pset);
  BOOST_CHECK_EQUAL(pset.get<int>("a"), 6);
  BOOST_CHECK_EQUAL(pset.get<int>("b"), 5);
  BOOST_CHECK_EQUAL(pset.get<int>("c"), 4);
}

BOOST_AUTO_TEST_CASE(overridden_nested_document)
{
  std::string document = "t : { a : 1\n"
                         "      b : 2\n"
                         "      a : 3\n"
                         "      c : 4\n"
                         "      b : 5\n"
                         "      a : 6\n"
                         "    }\n";
  intermediate_table tbl;
  BOOST_REQUIRE_NO_THROW(parse_document(document, tbl));
  ParameterSet pset;
  make_ParameterSet(tbl, pset);
  BOOST_CHECK_EQUAL(pset.get<int>("t.a"), 6);
  BOOST_CHECK_EQUAL(pset.get<int>("t.b"), 5);
  BOOST_CHECK_EQUAL(pset.get<int>("t.c"), 4);
}

BOOST_AUTO_TEST_CASE(nil_value)
{
  std::string document = "a : @nil\n"
                         "b : nil\n"
                         "c : \"@nil\"\n"
                         "d : \"nil\"\n"
                         "t : { a : @nil\n"
                         "      b : nil\n"
                         "      c : \"@nil\"\n"
                         "      d : \"nil\"\n"
                         "    }\n";
  intermediate_table tbl;
  BOOST_REQUIRE_NO_THROW(parse_document(document, tbl));
  ParameterSet pset;
  make_ParameterSet(tbl, pset);

  typedef void* nil_t;
  nil_t nil_value = 0;

  BOOST_CHECK_EQUAL(pset.get<nil_t>("a"), nil_value);
  BOOST_CHECK_EQUAL(pset.get<nil_t>("t.a"), nil_value);
  BOOST_CHECK_THROW(pset.get<string>("a"), fhicl::exception);
  BOOST_CHECK_THROW(pset.get<string>("t.a"), fhicl::exception);
  BOOST_CHECK_EQUAL(pset.get<string>("b"), "nil");
  BOOST_CHECK_EQUAL(pset.get<string>("t.b"), "nil");
  BOOST_CHECK_EQUAL(pset.get<string>("c"), "@nil");
  BOOST_CHECK_EQUAL(pset.get<string>("t.c"), "@nil");
  BOOST_CHECK_EQUAL(pset.get<string>("d"), "nil");
  BOOST_CHECK_EQUAL(pset.get<string>("t.d"), "nil");
}

BOOST_AUTO_TEST_CASE(erase_value)
{
  std::string document = "BEGIN_PROLOG\n"
                         "x: 27\n"
                         "z: 43\n"
                         "z: @erase\n"
                         "END_PROLOG\n"
                         "a: 27\n"
                         "b: { x: 7 y: 12 }\n"
                         "c: { x: 7 y: 12 x: @erase }\n"
                         "a: @erase\n"
                         "b.x: @erase\n";
  intermediate_table tbl;
  BOOST_REQUIRE_NO_THROW(parse_document(document, tbl));
  BOOST_CHECK(tbl.exists("x"));
  BOOST_CHECK(!tbl.exists("z"));
  BOOST_CHECK(!tbl.exists("a"));
  BOOST_CHECK(tbl.exists("b"));
  BOOST_CHECK(!tbl.exists("b.x"));
  BOOST_CHECK(tbl.exists("b.y"));
  BOOST_CHECK(tbl.exists("c"));
  BOOST_CHECK(!tbl.exists("c.x"));
  BOOST_CHECK(tbl.exists("c.y"));
}

BOOST_AUTO_TEST_CASE(expand_nested_tables)
{
  std::string document = "BEGIN_PROLOG\n"
                         "A: { B: { x: foo } }\n"
                         "END_PROLOG\n"
                         "modules: {\n"
                         "   A: {\n"
                         "      @table::A\n"
                         "      B:{ @table::A.B x:bar }\n"
                         "   }\n"
                         "}\n";
  intermediate_table tbl;
  parse_document(document, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::string>("modules.A.B.x"), std::string("bar"));
}

BOOST_AUTO_TEST_CASE(expand_table)
{
  std::string document = "BEGIN_PROLOG\n"
                         "fred: { bill: twelve charlie: 27 }\n"
                         "y: { @table::fred bill: \"one dozen\" }\n"
                         "END_PROLOG\n"
                         "x: { ethel: 14 bill: 12 @table::fred }\n"
                         "@table::fred\n"
                         "y: @local::y\n";
  intermediate_table tbl;
  parse_document(document, tbl);
  BOOST_CHECK(tbl.exists("x.ethel"));
  BOOST_CHECK(tbl.exists("x.charlie"));
  BOOST_CHECK(tbl.exists("bill"));
  BOOST_CHECK(tbl.exists("charlie"));
  BOOST_CHECK(tbl.exists("y.bill"));
  BOOST_CHECK(tbl.exists("y.charlie"));
  BOOST_CHECK_EQUAL(tbl.get<std::string>("x.bill"), std::string("twelve"));
  BOOST_CHECK_EQUAL(tbl.get<std::string>("bill"), std::string("twelve"));
  BOOST_CHECK_EQUAL(tbl.get<std::string>("y.bill"), std::string("one dozen"));
}

BOOST_AUTO_TEST_CASE(expand_sequence)
{
  std::string document = "BEGIN_PROLOG\n"
                         "fred: [ three, four, five ]\n"
                         "END_PROLOG\n"
                         "bill: [ one, two, @sequence::fred, six ]\n"
                         "charlie: @local::fred\n"
                         "ethel: [ @sequence::fred, six ]\n";
  intermediate_table tbl;
  parse_document(document, tbl);
  BOOST_CHECK(tbl.exists("fred"));
  BOOST_CHECK(tbl.exists("bill"));
  BOOST_CHECK(tbl.exists("charlie"));
  BOOST_CHECK(tbl.exists("ethel"));

  ParameterSet pset;
  make_ParameterSet(tbl, pset);

  BOOST_CHECK_EQUAL(pset.get<std::vector<std::string>>("charlie").size(), 3ul);

  std::vector<std::string> const billref{
    "one", "two", "three", "four", "five", "six"};
  std::vector<std::string> const ethelref{"three", "four", "five", "six"};

  auto cmp = [](std::vector<std::string> const& seq,
                std::vector<std::string> const& ref) {
    BOOST_CHECK_EQUAL(seq.size(), ref.size());
    for (auto i = seq.cbegin(), e = seq.cend(), iref = ref.cbegin(); i != e;
         ++i, ++iref) {
      BOOST_CHECK_EQUAL(*i, *iref);
    }
  };

  cmp(pset.get<std::vector<std::string>>("bill"), billref);
  cmp(pset.get<std::vector<std::string>>("ethel"), ethelref);
}

BOOST_AUTO_TEST_CASE(bad_lookup)
{
  std::string document = "x: @local::dead\n";
  intermediate_table tbl;
  BOOST_CHECK_THROW(parse_document(document, tbl), cet::exception);
}

BOOST_AUTO_TEST_CASE(bad_expand_table)
{
  std::string document = "BEGIN_PROLOG\n"
                         "bad: John\n"
                         "END_PROLOG\n"
                         "@table::bad\n";
  intermediate_table tbl;
  BOOST_CHECK_THROW(parse_document(document, tbl), cet::exception);
}

BOOST_AUTO_TEST_CASE(bad_expand_sequence)
{
  std::string document = "BEGIN_PROLOG\n"
                         "bad: John\n"
                         "END_PROLOG\n"
                         "f: [ @sequence::bad ]\n";
  intermediate_table tbl;
  BOOST_CHECK_THROW(parse_document(document, tbl), cet::exception);
}

BOOST_AUTO_TEST_CASE(colon_spacing)
{
  std::string const prolog = "BEGIN_PROLOG\n"
                             "t: { a: 7 b: 6}\n"
                             "s: [ 7, 6, 7 ]\n"
                             "END_PROLOG\n";
  intermediate_table tbl;
  std::vector<std::string> refs{
    "t1: @local::t\n",
    "@table::t\n",
    "s2: [ 1, 2, @sequence::s ]\n",
    "a: @id::0001020304050607080910111213141516171819\n",
    "t1: { t2: @local::t }\n",
    "t1: { @table::t }\n",
    "t1: { s1: [ 1, 2, 3, @sequence::s ] }\n",
    "t1: { a: @id::0001020304050607080910111213141516171819 }\n",
  };
  for (auto const& ref : refs) {
    BOOST_CHECK_NO_THROW(parse_document(prolog + ref, tbl));
    auto const cpos = ref.find("::");
    BOOST_REQUIRE(cpos != std::string::npos);
    std::string bad1{ref};
    std::string bad2{ref};
    bad1.insert(cpos, " ");
    BOOST_CHECK_THROW(parse_document(prolog + bad1, tbl), cet::exception);
    bad2.insert(cpos + 2, " ");
    BOOST_CHECK_THROW(parse_document(prolog + bad2, tbl), cet::exception);
  }
}

BOOST_AUTO_TEST_CASE(protect_ignore_01)
{
  std::string const doc = "x @protect_ignore: 29\n"
                          "x : 33\n"
                          "x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_02)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_ignore: 29\n"
                          "x : 33\n"
                          "END_PROLOG\n"
                          "x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("x"), 37ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_03)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_ignore: 29\n"
                          "END_PROLOG\n"
                          "x @protect_ignore: 33\n"
                          "x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("x"), 33ul);
}

#define PV_EXCEPTION                                                           \
  BOOST_CHECK_EXCEPTION(parse_document(doc, tbl),                              \
                        fhicl::exception,                                      \
                        [](fhicl::exception const& e) -> bool {                \
                          return e.categoryCode() ==                           \
                                   fhicl::error::parse_error &&                \
                                 e.root_cause() == "Protection violation";     \
                        })

BOOST_AUTO_TEST_CASE(protect_ignore_04)
{
  std::string const doc = "x @protect_ignore: 29\n"
                          "x @protect_ignore: 33\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_05)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_ignore: 29\n"
                          "x @protect_ignore: 33\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_06)
{
  std::string const doc = "a : { x @protect_ignore: 29 }\n"
                          "a.x : 33\n"
                          "a.x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_07)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a: { x @protect_ignore: 29 }\n"
                          "a.x : 33\n"
                          "END_PROLOG\n"
                          "a.x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 37ul);
  BOOST_CHECK(tbl.find("a.x").protection == fhicl::Protection::NONE);
}

BOOST_AUTO_TEST_CASE(protect_ignore_08)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { x @protect_ignore: 29 } \n"
                          "END_PROLOG\n"
                          "a.x @protect_ignore: 33\n"
                          "a.x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 33ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_09)
{
  std::string const doc = "a : { x @protect_ignore: 29 }\n"
                          "a.x @protect_ignore: 33\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_10)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { x @protect_ignore: 29 }\n"
                          "a.x @protect_ignore: 33\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_11)
{
  std::string const doc = "a @protect_ignore: { x : 29 }\n"
                          "a.x : 33\n"
                          "a.x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_12)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a @protect_ignore: { x : 29 }\n"
                          "a.x : 33\n"
                          "END_PROLOG\n"
                          "a.x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 37ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_13)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a @protect_ignore: { x : 29 }\n"
                          "END_PROLOG\n"
                          "a @protect_ignore: { x : 33 }\n"
                          "a.x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 33ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_14)
{
  std::string const doc = "a @protect_ignore: { x : 29 }\n"
                          "a @protect_ignore: { x : 33 }\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_15)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a @protect_ignore: { x : 29 }\n"
                          "a @protect_ignore: { x : 33 }\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_16)
{
  std::string const doc = "a : { b @protect_ignore: { x : 29 } }\n"
                          "a.b.x : 33\n"
                          "a.b.x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_17)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { b @protect_ignore: { x : 29 } }\n"
                          "a.b.x : 33\n"
                          "END_PROLOG\n"
                          "a : { b @protect_ignore: { x : 37 } }\n"
                          "a.b.x : 41\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.x"), 37ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_18)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { b @protect_ignore: { x : 29 } }\n"
                          "END_PROLOG\n"
                          "a : { b @protect_ignore: { x : 33 } }\n"
                          "a.b.x : 37\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.x"), 33ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_19)
{
  std::string const doc = "a : { b @protect_ignore: { x : 29 } }\n"
                          "a : { b @protect_ignore: { x : 33 } }\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.x"), 33ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_20)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { b @protect_ignore: { x : 29 } }\n"
                          "a : { b @protect_ignore: { x : 33 } }\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.x"), 33ul);
}

BOOST_AUTO_TEST_CASE(protect_ignore_21)
{
  std::string const doc = "a : { x : 29 }\n"
                          "a.x @protect_ignore: 33\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_ignore_22)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { x : 29 }\n"
                          "a.x @protect_ignore: 33\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_error_01)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_error: 29\n"
                          "x : 37\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_error_02)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_error: 29\n"
                          "END_PROLOG\n"
                          "x : 33\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("x"), 33ul);
}

BOOST_AUTO_TEST_CASE(protect_error_03)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_error: 29\n"
                          "END_PROLOG\n"
                          "x @protect_error: 33\n"
                          "x : 37\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_error_04)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_ignore: 29\n"
                          "x @protect_error: 33\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_error_05)
{
  std::string const doc = "x @protect_ignore: 29\n"
                          "x @protect_error: 33\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_error_06)
{
  std::string const doc = "a : { x @protect_error: 29 }\n"
                          "a : { x @protect_error: 33 }\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 33ul);
}

BOOST_AUTO_TEST_CASE(protect_error_07)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { x @protect_error: 29 }\n"
                          "a : { x @protect_error: 33 }\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.x"), 33ul);
}

BOOST_AUTO_TEST_CASE(protect_error_08)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { x @protect_error: 29 }\n"
                          "a.x : 33\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_error_09)
{
  std::string const doc = "a : { x @protect_error: 29 }\n"
                          "a.x : 33\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_error_10)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { b : { x @protect_error: 29 } } \n"
                          "a.b.x : 33\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_error_11)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "a : { b @protect_error: { x : 29 } }\n"
                          "a.b : { x : 33 }\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(erase_01)
{
  std::string const doc = "x : 29\n"
                          "x : @erase";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK(tbl.empty());
}

BOOST_AUTO_TEST_CASE(erase_02)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x : 29\n"
                          "x : @erase"
                          "END_PROLOG\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK(tbl.empty());
}

BOOST_AUTO_TEST_CASE(erase_03)
{
  std::string const doc = "x @protect_ignore: 29\n"
                          "x : @erase";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("x"), 29ul);
}

BOOST_AUTO_TEST_CASE(erase_04)
{
  std::string const doc = "x @protect_error: 29\n"
                          "x : @erase";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(erase_05)
{
  std::string const doc = "a : { b @protect_ignore: { x : 29 } }\n"
                          "a : @erase\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK(tbl.empty());
}

BOOST_AUTO_TEST_CASE(erase_06)
{
  std::string const doc = "a : { b @protect_ignore: { x : 29 } }\n"
                          "a.b : @erase\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(erase_07)
{
  std::string const doc = "a : { b @protect_ignore: { x : 29 } }\n"
                          "a.b.x : @erase\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(erase_08)
{
  std::string const doc = "a : { b @protect_ignore: { c: { x : 29 } } }\n"
                          "a.b.c : @erase\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.c.x"), 29ul);
}

BOOST_AUTO_TEST_CASE(erase_09)
{
  std::string const doc = "a : { b @protect_error: { c: { x : 29 } } }\n"
                          "a.b.c : @erase\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(erase_10)
{
  std::string const doc = "a : { b @protect_error: { c: { x : 29 } } }\n"
                          "a.b : @erase\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(erase_11)
{
  std::string const doc = "a : { b @protect_error: { x : 29 } }\n"
                          "a : @erase\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK(tbl.empty());
}

BOOST_AUTO_TEST_CASE(protect_local_01)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_ignore: 27\n"
                          "a: { b: { x: @local::x } }\n"
                          "END_PROLOG\n"
                          "a: @local::a\n"
                          "a.b.x: 29\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.x"), 27ul);
  BOOST_CHECK(tbl.find("a.b.x").protection == Protection::PROTECT_IGNORE);
}

BOOST_AUTO_TEST_CASE(protect_local_02)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_ignore: 27\n"
                          "a: { b: { x: @local::x } }\n"
                          "END_PROLOG\n"
                          "a @protect_error: @local::a\n"
                          "a.b.x: 29\n";
  intermediate_table tbl;
  PV_EXCEPTION;
}

BOOST_AUTO_TEST_CASE(protect_local_03)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x @protect_ignore: 27\n"
                          "a: { b: { x: @local::x } }\n"
                          "END_PROLOG\n"
                          "a @protect_ignore: @local::a\n"
                          "a.b.x: 29\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  BOOST_CHECK_EQUAL(tbl.get<std::size_t>("a.b.x"), 27ul);
  BOOST_CHECK(tbl.find("a.b.x").protection == Protection::PROTECT_IGNORE);
  BOOST_CHECK(tbl.find("a").protection == Protection::PROTECT_IGNORE);
  BOOST_CHECK(tbl.find("a.b.x").protection == Protection::PROTECT_IGNORE);
}

namespace {
  bool
  is_parse_error_at(fhicl::exception const& e, size_t line, size_t charpos)
  {
    bool result = e.categoryCode() == fhicl::error::parse_error;
    if (result) {
      using std::to_string;
      static std::regex const RE(
        "detected at or near line (\\d+), character (\\d+)");
      std::smatch sm;
      std::string match_string = e.what();
      result = std::regex_search(match_string, sm, RE) &&
               sm[1] == to_string(line) && sm[2] == to_string(charpos);
    }
    return result;
  }
}

#define PARSE_ERROR(line, charpos)                                             \
  BOOST_CHECK_EXCEPTION(                                                       \
    parse_document(doc, tbl),                                                  \
    fhicl::exception,                                                          \
    std::bind(&is_parse_error_at, std::placeholders::_1, line, charpos));

BOOST_AUTO_TEST_CASE(bad_par_01)
{
  std::string const doc = "x : 26\n"
                          "a 36\n";
  intermediate_table tbl;
  PARSE_ERROR(2, 1);
}

BOOST_AUTO_TEST_CASE(bad_par_02)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x : 26\n"
                          "a : 36\n"
                          "END_PROLOG\n"
                          "y : 26\n"
                          "b 36\n";
  intermediate_table tbl;
  PARSE_ERROR(6, 1);
}

BOOST_AUTO_TEST_CASE(bad_prolog)
{
  std::string const doc = "BEGIN_PROLOG\n"
                          "x : 26\n"
                          "a 36\n"
                          "END_PROLOG\n";
  intermediate_table tbl;
  PARSE_ERROR(3, 1);
}

BOOST_AUTO_TEST_SUITE_END()
