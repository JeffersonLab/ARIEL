#define BOOST_TEST_MODULE (maybe_ref test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/maybe_ref.h"

BOOST_AUTO_TEST_SUITE(maybe_ref)

BOOST_AUTO_TEST_CASE(default_behavior)
{
  {
    typedef int T;
    T x = 16;
    cet::maybe_ref<T> m(x);
    BOOST_CHECK(m.isValid());
    BOOST_CHECK(m.ref() == 16);
    T& r = m.ref();
    BOOST_CHECK(r == x);
    BOOST_CHECK(&r == &x);
    T const& r2 = m.ref();
    BOOST_CHECK(m.ref() == 16);
    BOOST_CHECK(r2 == x);
    BOOST_CHECK(&r2 == &x);
  }
  {
    typedef double T;
    cet::maybe_ref<T> m;
    BOOST_CHECK(!m.isValid());
    BOOST_CHECK_THROW(m.ref(), std::logic_error);
  }
}

BOOST_AUTO_TEST_CASE(copy_behavior)
{
  {
    typedef unsigned T;
    T x = 16u;
    cet::maybe_ref<T> m(x);
    BOOST_CHECK(m.isValid());
    BOOST_CHECK(m.ref() == 16u);
    cet::maybe_ref<T> m2 = m;
    BOOST_CHECK(m2.isValid());
    BOOST_CHECK(m2.ref() == 16u);
  }
  {
    typedef long T;
    T x = 16;
    cet::maybe_ref<T> m(x);
    BOOST_CHECK(m.isValid());
    BOOST_CHECK(m.ref() == 16);
    cet::maybe_ref<T> m2;
    BOOST_CHECK(!m2.isValid());
    m2 = m;
    BOOST_CHECK(m2.isValid());
    BOOST_CHECK(m2.ref() == 16);
  }
}

BOOST_AUTO_TEST_CASE(const_behavior)
{
  {
    typedef int const T;
    T x = 16;
    cet::maybe_ref<T> m(x);
    BOOST_CHECK(m.isValid());
    T& r = m.ref();
    BOOST_CHECK(r == x);
    BOOST_CHECK(&r == &x);
  }
}

BOOST_AUTO_TEST_CASE(reseat_behavior)
{
  {
    typedef int const T;
    T x = 16;
    T y = 48;
    cet::maybe_ref<T> m(x);
    BOOST_CHECK(m.isValid());
    BOOST_CHECK(m.ref() == 16);
    BOOST_CHECK(&m.ref() == &x);
    m.reseat();
    BOOST_CHECK(!m.isValid());
    m.reseat(y);
    BOOST_CHECK(m.isValid());
    BOOST_CHECK(m.ref() == 48);
    BOOST_CHECK(&m.ref() == &y);
  }
}

BOOST_AUTO_TEST_SUITE_END()
