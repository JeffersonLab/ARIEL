#define BOOST_TEST_MODULE (Product aggregation Test)
#include "cetlib/quiet_unit_test.hpp"

//=====================================================
// Unit-tests for non-CLHEP aggregate overloads
// N.B. art::PtrVector cannot be overloaded

#include "canvas/test/Persistency/Common/MockRun.h"
#include "cetlib/map_vector.h"
#include "cetlib/test_macros.h"

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std::string_literals;
using arttest::MockRun;

// The MockRun::get() function returns by value, and not reference.
// Using the bare CET_CHECK_EQUAL_COLLECTIONS will therefore cause
// much woe since two calls will be made to Run::get.  The solution is
// to make a copy (as is done in AGGREGATE_CHECK_EQUAL_COLLECTIONS).
#define AGGREGATE_CHECK_EQUAL_COLLECTIONS(test, ref)                           \
  auto t = test;                                                               \
  CET_CHECK_EQUAL_COLLECTIONS(t, ref)

namespace {

  double constexpr tolerance = std::numeric_limits<double>::epsilon();

  struct HoursPerWorker {
    HoursPerWorker(std::string const& name, unsigned const hours)
      : name_{name}, hours_{hours}
    {}
    std::string name_;
    unsigned hours_;

    void
    aggregate(HoursPerWorker const& b)
    {
      if (name_ != b.name_)
        return;
      hours_ += b.hours_;
    }
  };

  struct A {
    A(std::string const& name) : name_{name} {}
    std::string name_;
    void
    aggregate(A const&) const
    {}
  };
}

// ICKY...but there aren't many options with the boost unit test if
// the type is an std::pair
namespace std {

  ostream&
  operator<<(ostream& os, pair<string, unsigned> const& p)
  {
    os << '[' << p.first << ',' << p.second << ']';
    return os;
  }

  ostream&
  operator<<(ostream& os, pair<cet::map_vector_key, string> const& entry)
  {
    os << '[' << entry.first.asUint() << ',' << entry.second << ']';
    return os;
  }
}

BOOST_AUTO_TEST_SUITE(aggregate_t)

BOOST_AUTO_TEST_CASE(class_type)
{
  MockRun r1;
  r1.put<HoursPerWorker>(HoursPerWorker{"Sam", 12});
  r1.put<HoursPerWorker>(HoursPerWorker{"Sam", 14});
  auto sam = r1.get<HoursPerWorker>();
  BOOST_CHECK_EQUAL(sam.name_, "Sam");
  BOOST_CHECK_EQUAL(sam.hours_, 26u);

  MockRun r2;
  r2.put<HoursPerWorker>(HoursPerWorker{"John", 8});
  r2.put<HoursPerWorker>(HoursPerWorker{"Jim", 10});
  auto john = r2.get<HoursPerWorker>();
  BOOST_CHECK_EQUAL(john.name_, "John");
  BOOST_CHECK_EQUAL(john.hours_, 8u);
}

BOOST_AUTO_TEST_CASE(numbers)
{
  MockRun r;
  r.put<double>(14.3);
  r.put<double>(3.4);
  BOOST_CHECK_CLOSE_FRACTION(r.get<double>(), 17.7, tolerance);
}

BOOST_AUTO_TEST_CASE(vector)
{
  using nums_t = std::vector<int>;
  MockRun r;
  r.put<nums_t>(nums_t{1, 3, 5});
  r.put<nums_t>(nums_t{2, 4, 6});
  auto ref = {1, 3, 5, 2, 4, 6};
  AGGREGATE_CHECK_EQUAL_COLLECTIONS(r.get<nums_t>(), ref);
}

BOOST_AUTO_TEST_CASE(list)
{
  using chars_t = std::list<char>;
  MockRun r;
  r.put<chars_t>(chars_t{'a', 'b', 'c'});
  r.put<chars_t>(chars_t{'y', 'z'});
  auto ref = {'a', 'b', 'c', 'y', 'z'};
  AGGREGATE_CHECK_EQUAL_COLLECTIONS(r.get<chars_t>(), ref);
}

BOOST_AUTO_TEST_CASE(deque)
{
  using nums_t = std::deque<unsigned>;
  MockRun r;
  r.put<nums_t>(nums_t{0, 5, 8});
  r.put<nums_t>(nums_t{1, 2, 4});
  std::initializer_list<unsigned> const ref{0u, 5u, 8u, 1u, 2u, 4u};
  AGGREGATE_CHECK_EQUAL_COLLECTIONS(r.get<nums_t>(), ref);
}

BOOST_AUTO_TEST_CASE(array)
{
  using histo_t = std::array<int, 3>;
  MockRun r;
  r.put<histo_t>(histo_t{{1, 4, 7}});
  r.put<histo_t>(histo_t{{-1, 6, 92}});
  auto ref = {0, 10, 99};
  AGGREGATE_CHECK_EQUAL_COLLECTIONS(r.get<histo_t>(), ref);
}

BOOST_AUTO_TEST_CASE(map)
{
  using map_t = std::map<std::string, unsigned>;
  map_t const children{{"Billy", 7}, {"Josephine", 9}, {"Gregory", 0}};
  map_t const adults{{"Jennifer", 28}, {"Gregory", 47}, {"Ervin", 78}};
  MockRun r;
  r.put<map_t>(children);
  r.put<map_t>(adults);
  map_t const people{{"Billy", 7},
                     {"Ervin", 78},
                     {"Gregory", 0},
                     {"Jennifer", 28},
                     {"Josephine", 9}};
  AGGREGATE_CHECK_EQUAL_COLLECTIONS(r.get<map_t>(), people);
}

BOOST_AUTO_TEST_CASE(multimap)
{
  using map_t = std::multimap<std::string, unsigned>;
  map_t const children{{"Billy", 7}, {"Josephine", 9}, {"Gregory", 0}};
  map_t const adults{{"Jennifer", 28}, {"Gregory", 47}, {"Ervin", 78}};
  MockRun r;
  r.put<map_t>(children);
  r.put<map_t>(adults);
  map_t const people{{"Billy", 7},
                     {"Ervin", 78},
                     {"Gregory", 0},
                     {"Gregory", 47},
                     {"Jennifer", 28},
                     {"Josephine", 9}};
  AGGREGATE_CHECK_EQUAL_COLLECTIONS(r.get<map_t>(), people);
}

BOOST_AUTO_TEST_CASE(set)
{
  using set_t = std::set<std::string>;
  MockRun r;
  r.put<set_t>(set_t{"Brahms", "Beethoven"});
  r.put<set_t>(set_t{"Bach", "Brahms"});
  set_t const composers{"Bach", "Beethoven", "Brahms"};
  AGGREGATE_CHECK_EQUAL_COLLECTIONS(r.get<set_t>(), composers);
}

BOOST_AUTO_TEST_CASE(pair)
{
  using pair_t = std::pair<unsigned, double>;
  MockRun r;
  r.put<pair_t>(pair_t{0u, 14.});
  r.put<pair_t>(pair_t{6u, 20.});
  auto result = r.get<pair_t>();
  BOOST_CHECK_EQUAL(result.first, 6u);
  BOOST_CHECK_CLOSE_FRACTION(result.second, 34., tolerance);
}

BOOST_AUTO_TEST_CASE(tuple)
{
  using tuple_t = std::tuple<A, unsigned, double>;
  MockRun r;
  r.put<tuple_t>(tuple_t{"run1"s, 3, 1.2});
  r.put<tuple_t>(tuple_t{"run2"s, 74, 2.9});
  auto result = r.get<tuple_t>();
  BOOST_CHECK_EQUAL(std::get<A>(result).name_, "run1");
  BOOST_CHECK_EQUAL(std::get<unsigned>(result), 77u);
  BOOST_CHECK_CLOSE_FRACTION(std::get<double>(result), 4.1, tolerance);
}

BOOST_AUTO_TEST_CASE(map_vector)
{
  using mv_t = cet::map_vector<std::string>;
  using key_type = typename mv_t::key_type;
  mv_t primes;
  primes.insert({key_type{2}, "two"});
  primes.insert({key_type{3}, "three"});
  primes.insert({key_type{5}, "five"});

  mv_t more_nums;
  more_nums.insert({key_type{7}, "seven"});
  more_nums.insert({key_type{11}, "eleven"});
  more_nums.insert({key_type{13}, "thirteen"});

  MockRun r;
  r.put<mv_t>(primes);
  r.put<mv_t>(more_nums);

  mv_t ref;
  ref.insert({key_type{2}, "two"});
  ref.insert({key_type{3}, "three"});
  ref.insert({key_type{5}, "five"});
  ref.insert({key_type{7}, "seven"});
  ref.insert({key_type{11}, "eleven"});
  ref.insert({key_type{13}, "thirteen"});

  AGGREGATE_CHECK_EQUAL_COLLECTIONS(r.get<mv_t>(), ref);
}

BOOST_AUTO_TEST_CASE(string1)
{
  MockRun r;
  r.put<std::string>("howdy");
  r.put<std::string>("dowdy");
  BOOST_REQUIRE_EXCEPTION(
    r.get<std::string>(), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::ProductCannotBeAggregated;
    });
}

BOOST_AUTO_TEST_CASE(string2)
{
  MockRun r;
  r.put<std::string>("howdy");
  r.put<std::string>("howdy");
  BOOST_CHECK_EQUAL(r.get<std::string>(), "howdy");
}

BOOST_AUTO_TEST_SUITE_END()
