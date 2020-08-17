#define BOOST_TEST_MODULE (map_vector test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/container_algorithms.h"
#include "cetlib/map_vector.h"
#include <boost/test/tools/output_test_stream.hpp>
#include <iostream>
#include <string>

using cet::map_vector;
using cet::map_vector_key;

BOOST_TEST_DONT_PRINT_LOG_VALUE(map_vector<int>::const_iterator)
BOOST_TEST_DONT_PRINT_LOG_VALUE(map_vector<int>::iterator)

BOOST_AUTO_TEST_SUITE(map_vector_test)

class cout_redirect {
public:
  cout_redirect(std::streambuf* new_buf) : old_buf(std::cout.rdbuf(new_buf)) {}

  ~cout_redirect() { std::cout.rdbuf(old_buf); }

private:
  std::streambuf* old_buf;
};

BOOST_AUTO_TEST_CASE(key_test)
{
  {
    boost::test_tools::output_test_stream output;
    {
      cout_redirect guard(output.rdbuf());
      map_vector_key k(3);
      std::cout << k << std::endl;
    }
    BOOST_TEST(output.is_equal("3\n"));
  }

  {
    map_vector_key k1(11), k2(12);
    BOOST_TEST(k1 == k1);
    BOOST_TEST(k1 != k2);
    BOOST_TEST(k1 <= k2);
    BOOST_TEST(k1 < k2);
  }

  {
    map_vector_key k1(123u), k2(123uL);
    BOOST_TEST(k1 == k2);
  }
}

BOOST_AUTO_TEST_CASE(emptymap_test)
{
  map_vector_key k(3);
  {
    map_vector<int> m;
    BOOST_TEST(m.empty());
    BOOST_TEST(m.size() == 0u);
    BOOST_TEST(!m.has(k));
    BOOST_TEST(m.begin() == m.end());
    BOOST_TEST(m.find(k) == m.end());
    BOOST_TEST(m.getOrNull(k) == nullptr);
    BOOST_CHECK_THROW(m.getOrThrow(k), cet::exception);
    BOOST_CHECK_THROW(m.front(), cet::exception);
    BOOST_CHECK_THROW(m.back(), cet::exception);
  }

  {
    map_vector<int> const m;
    BOOST_TEST(m.empty());
    BOOST_TEST(m.size() == 0u);
    BOOST_TEST(!m.has(k));
    BOOST_TEST(m.begin() == m.end());
    BOOST_TEST(m.find(k) == m.end());
    BOOST_TEST(m.getOrNull(k) == nullptr);
    BOOST_CHECK_THROW(m.getOrThrow(k), cet::exception);
    BOOST_CHECK_THROW(m.front(), cet::exception);
    BOOST_CHECK_THROW(m.back(), cet::exception);
  }
}

BOOST_AUTO_TEST_CASE(nonemptymap_test)
{
  using value_t = int;
  map_vector<value_t> m;
  std::size_t sz(0);

  {
    map_vector_key k(1);
    value_t v(10 + k.asInt());
    m[k] = v;
    BOOST_TEST(!m.empty());
    BOOST_TEST(m.size() == ++sz);
    BOOST_TEST(m.begin() != m.end());
    BOOST_TEST(m.has(k));
    BOOST_TEST(m.find(k)->second == v);
    BOOST_TEST(*m.getOrNull(k) == v);
    BOOST_TEST(m.getOrThrow(k) == v);
  }

  {
    map_vector_key k(3);
    value_t v(10 + k.asInt());
    m[k] = v;
    BOOST_TEST(!m.empty());
    BOOST_TEST(m.size() == ++sz);
    BOOST_TEST(m.begin() != m.end());
    BOOST_TEST(m.has(k));
    BOOST_TEST(m.find(k)->second == v);
    BOOST_TEST(*m.getOrNull(k) == v);
    BOOST_TEST(m.getOrThrow(k) == v);
  }

  {
    map_vector_key k(5);
    value_t v(10 + k.asInt());
    m[k] = v;
    BOOST_TEST(!m.empty());
    BOOST_TEST(m.size() == ++sz);
    BOOST_TEST(m.begin() != m.end());
    BOOST_TEST(m.has(k));
    BOOST_TEST(m.find(k)->second == v);
    BOOST_TEST(*m.getOrNull(k) == v);
    BOOST_TEST(m.getOrThrow(k) == v);
  }

  {
    map_vector_key k(2);
    value_t v(10 + k.asInt());
    m[k] = v;
    BOOST_TEST(!m.empty());
    BOOST_TEST(m.size() == ++sz);
    BOOST_TEST(m.begin() != m.end());
    BOOST_TEST(m.has(k));
    BOOST_TEST(m.find(k)->second == v);
    BOOST_TEST(*m.getOrNull(k) == v);
    BOOST_TEST(m.getOrThrow(k) == v);
  }

  {
    // Test that insertion works
    auto m2 = m;
    BOOST_TEST(m2.size() == sz);

    auto result = m2.insert({map_vector_key{4}, value_t{14}});
    BOOST_TEST(result.second);
  }

  {
    // Insertion should fail for already-existing keys, and the
    // iterator to the existing element should be returned.
    auto m2 = m;
    BOOST_TEST(m2.size() == sz);

    auto result = m2.insert({map_vector_key{2}, value_t{13}});
    BOOST_TEST(!result.second);
    BOOST_TEST(result.first->second == 12);
  }

  {
    auto m2 = m, m3 = m;
    BOOST_TEST(m2.size() == m.size());

    // Now change the mapped values
    cet::for_all(m2, [](auto& pr) { pr.second = pr.second + 2; });

    // Attempt to insert all elements, even though they have the same
    // keys as in m3.
    m3.insert(m2.begin(), m2.end());

    // Verify that the inserts were not successful (that the values of
    // m3 have not changed).
    auto b = m.begin(), b2 = m3.begin();
    auto e = m.end();
    for (; b != e; ++b, ++b2) {
      BOOST_TEST(b->second == b2->second);
      BOOST_TEST(b->first.asInt() == b2->first.asInt());
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
