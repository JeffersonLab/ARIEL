#define BOOST_TEST_MODULE (search allowed configuration test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/TableFragment.h"
#include "fhiclcpp/types/Tuple.h"
#include "fhiclcpp/types/detail/SearchAllowedConfiguration.h"

#include <iostream>
#include <string>
#include <vector>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;
auto supports_key = SearchAllowedConfiguration::supports_key;

namespace {

  struct S {
    Atom<int> test{Name{"atom"}};
    Sequence<int, 2> seq{Name{"sequence"}};
    Tuple<int, double, bool> tuple{Name{"tuple"}};
    struct U {
      Atom<int> test{Name{"nested_atom"}};
    };
    Tuple<Sequence<int, 2>, bool> tuple2{Name{"tuple2"}};
    Table<U> table2{Name{"table2"}};
  };
}

BOOST_AUTO_TEST_SUITE(searchAllowedConfiguration_test)

BOOST_AUTO_TEST_CASE(table_t)
{
  Table<S> t{Name{"table"}};
  BOOST_TEST(supports_key(t, "atom"));
  BOOST_TEST(!supports_key(t, "table.atom"));
  BOOST_TEST(!supports_key(t, "table.sequence"));
  BOOST_TEST(supports_key(t, "sequence"));
  BOOST_TEST(supports_key(t, "sequence[0]"));
  BOOST_TEST(!supports_key(t, "[0]"));
  BOOST_TEST(supports_key(t, "table2"));
  BOOST_TEST(supports_key(t, "tuple2[0][1]"));
  BOOST_TEST(supports_key(t, "tuple2[1]"));
}

BOOST_AUTO_TEST_CASE(seqInSeq_t)
{
  Sequence<Sequence<int, 2>> s{Name{"nestedSequence"}};
  BOOST_TEST(supports_key(s, "[0]"));
  BOOST_TEST(supports_key(s, "[0][0]"));
  BOOST_TEST(supports_key(s, "[0][1]"));
}

BOOST_AUTO_TEST_SUITE_END()
