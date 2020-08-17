#define BOOST_TEST_MODULE (InputTag_t)
#include "cetlib/quiet_unit_test.hpp"

#include "canvas/Utilities/InputTag.h"

#include <set>
#include <string>

BOOST_AUTO_TEST_SUITE(InputTag_t)
BOOST_AUTO_TEST_CASE(InputTag_default_ctor)
{
  art::InputTag t;
  std::string empty;
  BOOST_TEST(t.label() == empty);
  BOOST_TEST(t.instance() == empty);
  BOOST_TEST(t.process() == empty);
}

BOOST_AUTO_TEST_CASE(InputTag_from_string)
{
  std::string empty;
  std::string mylabel("thisIsMyLabel");
  art::InputTag t(mylabel);
  BOOST_TEST(t.label() == mylabel);
  BOOST_TEST(t.instance() == empty);
  BOOST_TEST(t.process() == empty);
}

BOOST_AUTO_TEST_CASE(InputTag_from_two_strings)
{
  std::string empty;
  std::string mylabel("thisIsMylabel");
  std::string myinstance("someinstance");
  art::InputTag t(mylabel, myinstance);
  BOOST_TEST(t.label() == mylabel);
  BOOST_TEST(t.instance() == myinstance);
  BOOST_TEST(t.process() == empty);
}

BOOST_AUTO_TEST_CASE(InputTag_from_three_strings)
{
  std::string mylabel("thisIsMylabel");
  std::string myinstance("someinstance");
  std::string myprocess("aprocess");
  art::InputTag t(mylabel, myinstance, myprocess);
  BOOST_TEST(t.label() == mylabel);
  BOOST_TEST(t.instance() == myinstance);
  BOOST_TEST(t.process() == myprocess);
}

BOOST_AUTO_TEST_CASE(InputTag_from_one_cstring)
{
  const char* mylabel = "mylabel";
  std::string empty;
  art::InputTag t(mylabel);
  BOOST_TEST(t.label() == std::string(mylabel));
  BOOST_TEST(t.instance() == empty);
  BOOST_TEST(t.process() == empty);
}

std::string
grabLabel(art::InputTag const& t)
{
  return t.label();
}

BOOST_AUTO_TEST_CASE(InputTag_convert_from_string)
{
  std::string mylabel("alabel");
  std::string result = grabLabel(mylabel);
  BOOST_TEST(mylabel == result);
}

BOOST_AUTO_TEST_CASE(InputTag_convert_from_cstring)
{
  const char* mylabel = "alabel";
  std::string result = grabLabel(mylabel);
  BOOST_TEST(std::string(mylabel) == result);
}

BOOST_AUTO_TEST_CASE(InputTagEmptyFields_1)
{
  art::InputTag a1("alabel::aprocess");
  BOOST_TEST(a1.label() == std::string("alabel"));
  BOOST_TEST(a1.instance() == std::string());
  BOOST_TEST(a1.process() == std::string("aprocess"));
}

BOOST_AUTO_TEST_CASE(InputTagEmptyFields_2)
{
  art::InputTag a1(":aninstance:aprocess");
  BOOST_TEST(a1.label() == std::string());
  BOOST_TEST(a1.instance() == std::string("aninstance"));
  BOOST_TEST(a1.process() == std::string("aprocess"));
}

BOOST_AUTO_TEST_CASE(InputTagEmptyFields_3)
{
  art::InputTag a1("alabel:aninstance:");
  BOOST_TEST(a1.label() == std::string("alabel"));
  BOOST_TEST(a1.instance() == std::string("aninstance"));
  BOOST_TEST(a1.process() == std::string());
}

BOOST_AUTO_TEST_CASE(InputTagEmptyFields_4)
{
  art::InputTag a1("alabel:aninstance");
  BOOST_TEST(a1.label() == std::string("alabel"));
  BOOST_TEST(a1.instance() == std::string("aninstance"));
  BOOST_TEST(a1.process() == std::string());
}

BOOST_AUTO_TEST_CASE(InputTag_comparison)
{
  art::InputTag const a1{"alabel:aname:aprocess"};
  art::InputTag const a2{"alabel:aname:aprocess"};
  auto const a3{a1};
  art::InputTag const b1{"alabel:aname:anotherprocess"};
  art::InputTag const b2{"alabel:anothername:aprocess"};
  art::InputTag const b3{"anotherlabel:aname:aprocess"};
  BOOST_TEST(a1 == a2);
  BOOST_TEST(a1 == a3);
  BOOST_TEST(a2 == a3);
  BOOST_TEST(a3 == a1);
  BOOST_TEST(a1 != b1);
  BOOST_TEST(a1 != b2);
  BOOST_TEST(a1 != b3);
  BOOST_TEST(b1 != b2);
  BOOST_TEST(b1 != b3);
  BOOST_TEST(b2 != b3);
}

BOOST_AUTO_TEST_CASE(InputTag_set)
{
  std::set<art::InputTag> test;
  test.emplace("c", "i");
  test.emplace("a::");
  test.emplace("b");

  std::vector<art::InputTag> const ref{"a::", "b", "c:i:"};
  BOOST_TEST(test == ref, boost::test_tools::per_element{});

  // Test that an already present InputTag is not inserted again.
  BOOST_TEST(test.emplace("c:i").second == false);
}

BOOST_AUTO_TEST_SUITE_END()
