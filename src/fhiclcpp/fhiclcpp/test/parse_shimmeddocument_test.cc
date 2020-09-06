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
#include <string>

using namespace fhicl;
using namespace std;

BOOST_AUTO_TEST_SUITE(shimmeddocument_test)

using sequence_t = fhicl::extended_value::sequence_t;

/*
 * Test case activates the snippet mode; it must be the fist test case in this
 * test suite
 */
BOOST_AUTO_TEST_CASE(enable_snippet_mode)
{
  BOOST_TEST(::shims::isSnippetMode(true));
}

/*
 * Test case checks if values stored in prolog can be accessed
 */
BOOST_AUTO_TEST_CASE(can_access_prolog_01)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " a: 1\n"
                         " b: \"bb\"\n"
                         "END_PROLOG\n"
                         "c: 3\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 3);

  BOOST_TEST(fhicl_table.exists("a"));
  auto const& a = fhicl_table.find("a");
  BOOST_TEST(a.in_prolog == true);
  BOOST_TEST(std::any_cast<std::string>(a.value) == "1");

  BOOST_TEST(fhicl_table.exists("b"));
  auto const& b = fhicl_table.find("b");
  BOOST_TEST(b.in_prolog == true);
  BOOST_TEST(std::any_cast<std::string>(b.value) == "\"bb\"");

  BOOST_TEST(fhicl_table.exists("c"));
  auto const& c = fhicl_table.find("c");
  BOOST_TEST(c.in_prolog == false);
  BOOST_TEST(std::any_cast<std::string>(c.value) == "3");
}

/*
 * Test case checks if values can be retried in the same order as they appear
 * in the fhicl document.
 */
BOOST_AUTO_TEST_CASE(preserved_order_of_values_01)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " z: 1.1\n"
                         " a: aa\n"
                         "END_PROLOG\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 2);

  auto it = fhicl_table.begin();

  BOOST_TEST(it->second.in_prolog == true);
  BOOST_TEST(it->first == "z");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "1.1");

  std::advance(it, 1);

  BOOST_TEST(it->second.in_prolog == true);
  BOOST_TEST(it->first == "a");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"aa\"");
}

/*
 * Test case checks if values can be retried in the same order as they appear
 * in the fhicl document.
 */
BOOST_AUTO_TEST_CASE(preserved_order_of_values_02)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "z: 1.1\n"
                         "a: aa\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 2);

  auto it = fhicl_table.begin();

  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "z");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "1.1");

  std::advance(it, 1);

  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "a");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"aa\"");
}

/*
 * Test case checks if values can be retried in the same order as they appear
 * in the fhicl document.
 */
BOOST_AUTO_TEST_CASE(preserved_order_of_values_03)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " z: 1.1\n"
                         " a: aa\n"
                         "END_PROLOG\n"
                         "x:xx\n"
                         "c:cc\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 4);

  auto it = fhicl_table.begin();
  BOOST_TEST(it->second.in_prolog == true);
  BOOST_TEST(it->first == "z");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "1.1");

  std::advance(it, 1);
  BOOST_TEST(it->second.in_prolog == true);
  BOOST_TEST(it->first == "a");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"aa\"");

  std::advance(it, 1);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "x");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"xx\"");

  std::advance(it, 1);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "c");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"cc\"");
}

/*
 * Test case checks if the resolving of @local::-style references can be turned
 * off
 */
BOOST_AUTO_TEST_CASE(keep_unresolved_references_01)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "z: 1.1\n"
                         "ss: {rr: zz}\n"
                         "mm: @local::ss\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 3);

  BOOST_TEST(fhicl_table.exists("mm"));
  auto const& mm = fhicl_table.find("mm");
  BOOST_TEST(mm.in_prolog == false);
  BOOST_TEST(std::any_cast<std::string>(mm.value) == "\"@local::ss\"");
}

/*
 * Test case checks if the resolving of @local::-style references can be turned
 * off
 */
BOOST_AUTO_TEST_CASE(keep_unresolved_references_02)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " z: 1.1\n"
                         " ss: {rr: zz}\n"
                         "END_PROLOG\n"
                         "mm: @local::ss\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 3);

  BOOST_TEST(fhicl_table.exists("mm"));
  auto const& mm = fhicl_table.find("mm");
  BOOST_TEST(mm.in_prolog == false);
  BOOST_TEST(std::any_cast<std::string>(mm.value) == "\"@local::ss\"");
}

/*
 * Test case checks if the resolving of @table::-style references can be turned
 * off
 */
BOOST_AUTO_TEST_CASE(keep_unresolved_references_03)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " tt: { a:1 b: 2 }\n"
                         "END_PROLOG\n"
                         "placeholder_table_001:@table::tt\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(fhicl_table.exists("placeholder_table_001"));

  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 2);

  auto const& tt = fhicl_table.find("placeholder_table_001");
  BOOST_TEST(tt.in_prolog == false);
  BOOST_TEST(std::any_cast<std::string>(tt.value) == "\"@table::tt\"");
}

/*
 * Test case checks if the resolving of @table::-style references can be turned
 * off
 */
BOOST_AUTO_TEST_CASE(keep_unresolved_references_04)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " tt: { a:1 b: 2 }\n"
                         " placeholder_table_001:@table::tt\n"
                         "END_PROLOG\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 2);

  BOOST_TEST(fhicl_table.exists("placeholder_table_001"));

  auto const& tt = fhicl_table.find("placeholder_table_001");
  BOOST_TEST(tt.in_prolog == true);
  BOOST_TEST(std::any_cast<std::string>(tt.value) == "\"@table::tt\"");
}

/*
 * Test case checks if the resolving of @sequence::-style references can be
 * turned off
 */
BOOST_AUTO_TEST_CASE(keep_unresolved_references_05)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " z: 1.1\n"
                         " ss: [23,24,25]\n"
                         "END_PROLOG\n"
                         "sss:[@sequence::ss, 34, 35]\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 3);

  auto const& sss = fhicl_table.find("sss");
  BOOST_TEST(sss.in_prolog == false);
  sequence_t seq = sss;
  BOOST_TEST(std::any_cast<std::string>(seq.at(0).value) ==
             "\"@sequence::ss\"");
}

/*
 * Test case checks if the resolving of @sequence::-style references can be
 * turned off
 */
BOOST_AUTO_TEST_CASE(keep_unresolved_references_06)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " z: 1.1\n"
                         " ss: [23,24,25]\n"
                         "sss:[@sequence::ss, 34, 35]\n"
                         "END_PROLOG\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));
  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 3);

  auto const& sss = fhicl_table.find("sss");
  BOOST_TEST(sss.in_prolog == true);
  sequence_t seq = sss;
  BOOST_TEST(std::any_cast<std::string>(seq.at(0).value) ==
             "\"@sequence::ss\"");
}

/*
 * Test case checks if the @erase directive can be turned off
 */
BOOST_AUTO_TEST_CASE(keep_unresolved_references_07)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " z: 1.1\n"
                         " ss: {rr: zz}\n"
                         "END_PROLOG\n"
                         "mm:@local::ss\n"
                         "mm:@erase\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));
  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 4);

  BOOST_TEST(fhicl_table.exists("mm"));

  auto const& mm = fhicl_table.find("mm");
  BOOST_TEST(mm.in_prolog == false);
  BOOST_TEST(std::any_cast<std::string>(mm.value) == "\"@local::ss\"");

  auto it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 1);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "mm");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"@erase\"");
}

/*
 * Test case checks if the "@nil" values are not affected by the parsing shim
 * code
 */
BOOST_AUTO_TEST_CASE(keep_nil_01)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " a: @nil\n"
                         " ss: {rr: zz}\n"
                         "END_PROLOG\n"
                         "mm:@nil\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));

  BOOST_TEST(fhicl_table.exists("mm"));
  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 3);

  auto const& mm = fhicl_table.find("mm");
  BOOST_TEST(mm.in_prolog == false);
  BOOST_TEST(mm.is_a(::fhicl::NIL));

  auto const& a = fhicl_table.find("a");
  BOOST_TEST(a.in_prolog == true);
  BOOST_TEST(a.is_a(::fhicl::NIL));
}

/*
 * Test case checks if the dot delimiter processing can be turned off
 */
BOOST_AUTO_TEST_CASE(keep_dots_in_keynames_01)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " z: 1.1\n"
                         " ss: {rr: zz\n"
                         " h: { hh: {hhh:hhhh } } }\n"
                         "END_PROLOG\n"
                         "h.hh.hhh:h\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));
  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 3);

  BOOST_TEST(fhicl_table.exists("h.hh.hhh"));

  auto const& h = fhicl_table.find("h.hh.hhh");
  BOOST_TEST(h.in_prolog == false);
  BOOST_TEST(std::any_cast<std::string>(h.value) == "\"h\"");
}

/*
 * Test case checks if the dot delimiter processing can be turned off
 */
BOOST_AUTO_TEST_CASE(keep_dots_in_keynames_02)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "BEGIN_PROLOG\n"
                         " z: 1.1\n"
                         " c:c1\n"
                         "END_PROLOG\n"
                         "h.hh.hhh:h\n"
                         "c.cc.ccc:@local::c\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));
  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 4);

  BOOST_TEST(fhicl_table.exists("c"));

  auto const& c = fhicl_table.find("c");
  BOOST_TEST(c.in_prolog == true);
  BOOST_TEST(std::any_cast<std::string>(c.value) == "\"c1\"");

  auto it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 2);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "h.hh.hhh");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"h\"");

  it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 1);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "c.cc.ccc");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"@local::c\"");
}

/*
 * Test case checks if duplicate key value pairs allowed
 */
BOOST_AUTO_TEST_CASE(allow_duplicate_kvp_01)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "c:c1\n"
                         "c:c2\n"
                         "h:h1\n"
                         "c:c3\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));
  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 4);

  BOOST_TEST(fhicl_table.exists("c"));

  auto const& c = fhicl_table.find("c");
  BOOST_TEST(c.in_prolog == false);
  BOOST_TEST(std::any_cast<std::string>(c.value) == "\"c1\"");

  auto it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 3);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "c");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"c2\"");

  it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 2);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "h");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"h1\"");

  it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 1);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "c");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"c3\"");
}

/*
 * Test case checks if duplicate key value pairs allowed
 */
BOOST_AUTO_TEST_CASE(allow_duplicate_kvp_02)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "c.cc:c1\n"
                         "c.cc:c2\n"
                         "h:h1\n"
                         "c.cc:c3\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));
  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 4);

  BOOST_TEST(fhicl_table.exists("c.cc"));

  auto const& c = fhicl_table.find("c.cc");
  BOOST_TEST(c.in_prolog == false);
  BOOST_TEST(std::any_cast<std::string>(c.value) == "\"c1\"");

  auto it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 3);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "c.cc");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"c2\"");

  it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 2);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "h");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"h1\"");

  it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 1);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "c.cc");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"c3\"");
}

/*
 * Test case checks if duplicate key value pairs allowed
 */
BOOST_AUTO_TEST_CASE(allow_duplicate_kvp_03)
{
  BOOST_TEST(::shims::isSnippetMode());

  std::string document = "c.cc:c1\n"
                         "c.cc:c2\n"
                         "h:h1\n"
                         "c.cc:@local::h\n";

  ::fhicl::intermediate_table fhicl_table;

  BOOST_REQUIRE_NO_THROW(::fhicl::parse_document(document, fhicl_table));
  BOOST_TEST(std::distance(fhicl_table.begin(), fhicl_table.end()) == 4);

  BOOST_TEST(fhicl_table.exists("c.cc"));

  auto const& c = fhicl_table.find("c.cc");
  BOOST_TEST(c.in_prolog == false);
  BOOST_TEST(std::any_cast<std::string>(c.value) == "\"c1\"");

  auto it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 3);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "c.cc");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"c2\"");

  it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 2);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "h");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"h1\"");

  it = fhicl_table.begin();
  std::advance(it, std::distance(fhicl_table.begin(), fhicl_table.end()) - 1);
  BOOST_TEST(it->second.in_prolog == false);
  BOOST_TEST(it->first == "c.cc");
  BOOST_TEST(std::any_cast<std::string>(it->second.value) == "\"@local::h\"");
}

BOOST_AUTO_TEST_SUITE_END()
