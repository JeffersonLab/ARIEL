#define BOOST_TEST_MODULE (TypeID_t)
#include "boost/test/tools/output_test_stream.hpp"
#include "cetlib/quiet_unit_test.hpp"

#include "canvas/Utilities/TypeID.h"

#include <iostream>
#include <string>

namespace arttest {
  struct empty {};
  struct also_empty {};
}

BOOST_AUTO_TEST_SUITE(TypeID_t)

BOOST_AUTO_TEST_CASE(TypeID_equality_val)
{
  arttest::empty e;
  art::TypeID id1(typeid(e));
  art::TypeID id2(typeid(e));

  BOOST_TEST(!(id1 < id2));
  BOOST_TEST(!(id2 < id1));

  BOOST_TEST(id1 == id2);

  std::string n1(id1.name());
  std::string n2(id2.name());

  BOOST_TEST(n1 == n2);
}

BOOST_AUTO_TEST_CASE(TypeID_copy_val)
{
  arttest::empty e;
  art::TypeID id1(typeid(e));

  art::TypeID id3 = id1;
  BOOST_TEST(!(id1 < id3));
  BOOST_TEST(!(id3 < id1));

  BOOST_TEST(id1 == id3);

  std::string n1(id1.name());
  std::string n3(id3.name());
  BOOST_TEST(n1 == n3);
}

BOOST_AUTO_TEST_CASE(TypeID_type)
{
  art::TypeID id1(typeid(arttest::empty));
  art::TypeID id2(typeid(arttest::also_empty));

  boost::test_tools::output_test_stream os;

  BOOST_TEST(id1 != id2);

  os << id1;
  BOOST_TEST(os.is_equal("arttest::empty"));

  os.clear();
  os << id2;
  BOOST_TEST(os.is_equal("arttest::also_empty"));
}

BOOST_AUTO_TEST_SUITE_END()
