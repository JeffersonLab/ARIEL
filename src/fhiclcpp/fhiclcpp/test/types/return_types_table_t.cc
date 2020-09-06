// ======================================================================
//
// test types return type for tables

/* The purpose of this test is to verify that types 14-21 below return
   the correct types.

   In what follows, ’T’ represents a type supported by an Atom<>
   and ’S’ represents an explicitly constructed struct that may
   contain Atom<>, Sequence<>, Tuple<>, or Table<> objects.

   [ 1] Atom<T>;
   [ 2] Sequence<T>
   [ 3] Sequence<T,SZ>
   [ 4] Tuple<T...>
   [ 5] Tuple< Sequence<T>, U...>
   [ 6] Tuple< Sequence<T,SZ>, U...>
   [ 7] Tuple< Tuple<T...>,U...>
   [ 8] Sequence< Tuple<T...> >
   [ 9] Sequence< Tuple<T...>, SZ >
   [10] Sequence< Sequence<T> >
   [11] Sequence< Sequence<T,SZ> >
   [12] Sequence< Sequence<T>, SZ >
   [13] Sequence< Sequence<T,SZ>, SZ >

   14-21 cannot support default arguments (since Table<> cannot have a default)

   [14] Table<S>
   [15] Sequence< Table<S> >
   [16] Sequence< Table<S>, SZ >
   [17] Tuple< Table<S>, U... >
   [18] Tuple< Sequence< Table<S> >, U... >
   [19] Tuple< Sequence< Table<S>, SZ>, U... >
   [20] Sequence< Tuple< Table<S>, U... > >
   [21] Sequence< Tuple< Table<S>, U... >, SZ>

 */

// ======================================================================

#define BOOST_TEST_MODULE (return types with tables)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/Tuple.h"

#include <iostream>
#include <string>
#include <vector>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;

namespace {

  template <typename R, typename T>
  void
  require_type_as(T& t [[maybe_unused]])
  {
    auto rt = t();
    BOOST_TEST((std::is_same<R, decltype(rt)>::value));
  }

  struct S {
    Atom<int> test{Name("atom")};
    Sequence<int, 2> seq{Name("sequence")};
    Tuple<int, double, bool> tuple{Name("tuple")};
  };
}

BOOST_AUTO_TEST_SUITE(types_return_types_tables)

// [14] Table<S>
BOOST_AUTO_TEST_CASE(table_t)
{
  Table<S> test{Name("table")};
  require_type_as<S>(test);
}

// [15] Sequence< Table<S> >
BOOST_AUTO_TEST_CASE(table_in_seq_t)
{
  Sequence<Table<S>> test{Name("seqtable")};
  require_type_as<std::vector<S>>(test);
}

// [16] Sequence< Table<S>,2 >
BOOST_AUTO_TEST_CASE(table_in_seq_2_t)
{
  Sequence<Table<S>, 2> test{Name("seqtable")};
  require_type_as<std::array<S, 2>>(test);
}

// [17] Tuple< Table<S>, U... >
BOOST_AUTO_TEST_CASE(table_in_tuple_t)
{
  Tuple<Table<S>, int, double> test{Name("tuptable")};
  require_type_as<std::tuple<S, int, double>>(test);
}

// [18] Tuple< Sequence< Table<S> >, U... >
BOOST_AUTO_TEST_CASE(seqtable_in_tuple_t)
{
  Tuple<Sequence<Table<S>>, int, double> test{Name("seqtuptable")};
  require_type_as<std::tuple<std::vector<S>, int, double>>(test);
}

// [19] Tuple< Sequence< Table<S>, SZ >, U... >
BOOST_AUTO_TEST_CASE(seqtable_2_in_tuple_t)
{
  Tuple<Sequence<Table<S>, 2>, int, double> test{Name("seqtuptable")};
  require_type_as<std::tuple<std::array<S, 2>, int, double>>(test);
}

// [20] Sequence< Tuple< Table<S>, U... > >
BOOST_AUTO_TEST_CASE(tuptable_in_seq_t)
{
  Sequence<Tuple<Table<S>, int, double>> test{Name("tupseqtable")};
  require_type_as<std::vector<std::tuple<S, int, double>>>(test);
}

// [21] Sequence< Tuple< Table<S>, U... >, SZ >
BOOST_AUTO_TEST_CASE(tuptable_in_seq_2_t)
{
  Sequence<Tuple<Table<S>, int, double>, 2> test{Name("tupseqtable")};
  require_type_as<std::array<std::tuple<S, int, double>, 2>>(test);
}

BOOST_AUTO_TEST_SUITE_END()
