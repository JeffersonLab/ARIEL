// ======================================================================
//
// test table return values from FHiCL files
//
//    The purpose of this test is to verify that all table values
//    are accurately retrieved after being validated.
//
// ======================================================================

#define BOOST_TEST_MODULE (return table values from fcl)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/TableFragment.h"
#include "fhiclcpp/types/Tuple.h"

#include "fhiclcpp/test/types/FixtureBase.h"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace fhicl;
using namespace fhicl::detail;
using namespace std;

namespace {

  double const tolerance = std::numeric_limits<double>::epsilon();

  struct RefS {

    RefS(int const i,
         int const sj,
         int const sk,
         int const tl,
         string const str,
         bool const flag)
      : i_(i), sj_(sj), sk_(sk), tl_(tl), str_(str), flag_(flag)
    {}

    int i_;
    int sj_;
    int sk_;
    int tl_;
    string str_;
    bool flag_;
  };

  std::ostream& operator<<[[gnu::unused]](std::ostream& os, RefS const& refs)
  {
    os << " Atom: " << refs.i_ << " Sequence: [ " << refs.sj_ << ", "
       << refs.sk_ << " ]"
       << " Tuple: [ " << refs.tl_ << ", " << refs.str_ << ", "
       << std::boolalpha << refs.flag_ << " ]";
    return os;
  }

  struct S {
    Atom<int> atom{Name("atom")};
    Sequence<int, 2> sequence{Name("sequence")};
    Tuple<int, string, bool> tuple{Name("tuple")};

    bool
    operator==(const RefS& refs) const
    {
      return this->atom() == refs.i_ && this->sequence(0) == refs.sj_ &&
             this->sequence(1) == refs.sk_ &&
             this->tuple.get<0>() == refs.tl_ &&
             this->tuple.get<1>() == refs.str_ &&
             this->tuple.get<2>() == refs.flag_;
    }
  };

  std::ostream& operator<<[[gnu::unused]](std::ostream& os, S const& s)
  {
    os << " Atom: " << s.atom() << " Sequence: [ " << s.sequence(0) << ", "
       << s.sequence(1) << " ]"
       << " Tuple: [ " << s.tuple.get<0>() << ", " << s.tuple.get<1>() << ", "
       << std::boolalpha << s.tuple.get<2>() << " ]";
    return os;
  }

  struct Configuration {
    Table<S> table{Name("table")};
    Sequence<Table<S>> vecOfTable{Name("vecOfTable")};
    Sequence<Table<S>, 2> arrOfTable{Name("arrOfTable")};
    Tuple<Table<S>, int, double> tupWithTable{Name("tupWithTable")};
    Tuple<Sequence<Table<S>>, int, double> tupWithVecTable{
      Name("tupWithVecTable")};
    Tuple<Sequence<Table<S>, 2>, int, double> tupWithArrTable{
      Name("tupWithArrTable")};
    Sequence<Tuple<Table<S>, int, double>> vecWithTupTable{
      Name("vecWithTupTable")};
    Sequence<Tuple<Table<S>, int, double>, 2> arrWithTupTable{
      Name("arrWithTupTable")};
    TableFragment<S> tFragment;
  };

  struct Fixture : fhiclcpp_types::FixtureBase<Configuration> {
    Fixture() : FixtureBase("return_table_values_from_fcl_t.fcl") {}
  };
}

// provide use of 'Table<Configuration> config'
BOOST_FIXTURE_TEST_SUITE(values_from_fcl, Fixture)

// [14] Table<S>
BOOST_AUTO_TEST_CASE(table_t)
{
  RefS ref(4, 3, 6, 8, "something", false);
  BOOST_CHECK_EQUAL(config().table(), ref);
}

// [15] Sequence< Table<S> >
BOOST_AUTO_TEST_CASE(table_in_seq_t)
{
  auto ref = {RefS{0, 10, 100, 0, "something0", true}};
  auto it = ref.begin();

  for (auto const& table : config().vecOfTable())
    BOOST_CHECK_EQUAL(table, *it++);
}

// [16] Sequence< Table<S>,2 >
BOOST_AUTO_TEST_CASE(table_in_seq_2_t)
{
  auto ref = {RefS{0, 10, 100, 0, "array0", true},
              RefS{1, 11, 101, 1, "array1", true}};

  auto it = ref.begin();

  for (auto const& table : config().arrOfTable())
    BOOST_CHECK_EQUAL(table, *it++);
}

// [17] Tuple< Table<S>, U... >
BOOST_AUTO_TEST_CASE(table_in_tuple_t)
{
  RefS ref{3, 13, 103, 3, "tup0", true};
  BOOST_CHECK_EQUAL(config().tupWithTable.get<0>(), ref);
  BOOST_CHECK_EQUAL(config().tupWithTable.get<1>(), 981);
  BOOST_CHECK_CLOSE_FRACTION(config().tupWithTable.get<2>(), 581.1, tolerance);
}

// [18] Tuple< Sequence< Table<S> >, U... >
BOOST_AUTO_TEST_CASE(vec_table_in_tuple_t)
{
  auto ref = {RefS{4, 14, 104, 4, "tup0", true}};
  auto it = ref.begin();
  for (auto const& table : config().tupWithVecTable.get<0>())
    BOOST_CHECK_EQUAL(table, *it++);
  BOOST_CHECK_EQUAL(config().tupWithVecTable.get<1>(), 345);
  BOOST_CHECK_CLOSE_FRACTION(
    config().tupWithVecTable.get<2>(), 234.14, tolerance);
}

// [19] Tuple< Sequence< Table<S>, SZ >, U... >
BOOST_AUTO_TEST_CASE(arr_table_in_tuple_t)
{
  auto ref = {RefS{5, 15, 105, 5, "tup0", true},
              RefS{6, 16, 106, 6, "tup1", true}};
  auto it = ref.begin();

  for (auto const& table : config().tupWithArrTable.get<0>())
    BOOST_CHECK_EQUAL(table, *it++);
  BOOST_CHECK_EQUAL(config().tupWithArrTable.get<1>(), 789);
  BOOST_CHECK_CLOSE_FRACTION(
    config().tupWithArrTable.get<2>(), 17.06, tolerance);
}

// [20] Sequence< Tuple< Table<S>, U... > >
BOOST_AUTO_TEST_CASE(tup_table_in_vec_t)
{
  auto ref = RefS{7, 17, 107, 7, "tup0", true};

  for (auto const& tup : config().vecWithTupTable()) {
    BOOST_CHECK_EQUAL(std::get<0>(tup), ref);
    BOOST_CHECK_EQUAL(std::get<1>(tup), 4);
    BOOST_CHECK_CLOSE_FRACTION(std::get<2>(tup), 1.0004, tolerance);
  }
}

// [21] Sequence< Tuple< Table<S>, U... >, SZ >
BOOST_AUTO_TEST_CASE(tup_table_in_arr_t)
{
  auto ref_ts = {RefS{8, 18, 108, 8, "tup0", true},
                 RefS{9, 19, 109, 9, "tup1", true}};
  auto ref_is = {4, 40};
  auto ref_ds = {0.89881, 1.47412};

  auto it_ts = ref_ts.begin();
  auto it_is = ref_is.begin();
  auto it_ds = ref_ds.begin();

  for (auto const& tup : config().arrWithTupTable()) {
    BOOST_CHECK_EQUAL(std::get<0>(tup), *it_ts++);
    BOOST_CHECK_EQUAL(std::get<1>(tup), *it_is++);
    BOOST_CHECK_CLOSE_FRACTION(std::get<2>(tup), *it_ds++, tolerance);
  }
}

// [22] TableFragment<S>
BOOST_AUTO_TEST_CASE(tableFragment_t)
{
  auto ref = RefS{10, 20, 200, 10, "tup", false};
  BOOST_CHECK_EQUAL(config().tFragment(), ref);
}

BOOST_AUTO_TEST_SUITE_END()
