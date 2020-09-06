#define BOOST_TEST_MODULE (KeyAssembler test)

#include "cetlib/quiet_unit_test.hpp"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <set>
#include <string>
#include <vector>

using namespace fhicl;

// KeyAssembler is used by ParameterSet::get_all_keys()

BOOST_AUTO_TEST_SUITE(key_assembler_test)

BOOST_AUTO_TEST_CASE(t1)
{
  std::string const doc = "p1: {}\n"
                          "p3: {\n"
                          "   a: else\n"
                          "   b: []\n"
                          "   d: [ 11, 12 ]\n"
                          "   p4: { e: f }\n"
                          "   g: [ {h1: i1}, {h2: i2} ]\n"
                          "}\n";
  intermediate_table tbl;
  parse_document(doc, tbl);
  ParameterSet pset;
  make_ParameterSet(tbl, pset);
  auto const keys = pset.get_all_keys();
  std::set<std::string> const sorted_keys{keys.begin(), keys.end()};

  auto const ref = {"p1",
                    "p3",
                    "p3.a",
                    "p3.b",
                    "p3.d",
                    "p3.d[0]",
                    "p3.d[1]",
                    "p3.p4",
                    "p3.p4.e",
                    "p3.g",
                    "p3.g[0]",
                    "p3.g[0].h1",
                    "p3.g[1]",
                    "p3.g[1].h2"};
  std::set<std::string> const sorted_ref{ref.begin(), ref.end()};
  BOOST_TEST(sorted_keys == sorted_ref);
}

BOOST_AUTO_TEST_SUITE_END()
