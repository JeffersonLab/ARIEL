// ======================================================================
//
// test value_ptr
//
// ======================================================================

#define BOOST_TEST_MODULE (value_ptr test)
#include "cetlib/quiet_unit_test.hpp"
#include <string>

#include "cetlib/value_ptr.h"

using cet::default_clone;
using cet::default_copy;
using cet::value_ptr;

struct Base {
  virtual std::string
  who() const
  {
    return "Base";
  }
  virtual ~Base() noexcept = default;
};

struct Derived : public Base {
  std::string
  who() const override
  {
    return "Derived";
  }
};

BOOST_AUTO_TEST_SUITE(value_ptr_test)

BOOST_AUTO_TEST_CASE(nullptr_test)
{
  value_ptr<int> p;
  BOOST_CHECK(!p);
  BOOST_CHECK(p == nullptr);
  BOOST_CHECK(nullptr == p);
  BOOST_CHECK(p == 0);
  BOOST_CHECK(0 == p);
}

BOOST_AUTO_TEST_CASE(basic_test)
{
  value_ptr<int> p(new int(16));
  BOOST_CHECK(bool(p));
  BOOST_CHECK_EQUAL(*p, 16);

  int* p_addr = p.get();
  value_ptr<int> q(p);
  BOOST_CHECK_EQUAL(*p, *q);
  BOOST_CHECK(p != q);
  BOOST_CHECK(p_addr != q.get());

  p.reset(new int(0));
  BOOST_CHECK_EQUAL(*p, 0);
  BOOST_CHECK(p_addr != p.get());
}

BOOST_AUTO_TEST_CASE(compile_failure_test)
{
#if 0
  value_ptr<double> p( new int(16) ); // unrelated types
  value_ptr<Base> b( new Derived );   // polymorphic, but no clone()
  value_ptr<Derived> d( new Base );   // no conversion in this direction
#endif
}

BOOST_AUTO_TEST_CASE(polymorphism_test)
{
  value_ptr<Base, default_copy<Base>> b(new Base);
  BOOST_CHECK(bool(b));
  BOOST_CHECK_EQUAL(b->who(), std::string("Base"));

  value_ptr<Base, default_copy<Base> // request slicing
            >
    d(new Derived);
  BOOST_CHECK(bool(d));
  BOOST_CHECK_EQUAL(d->who(), std::string("Derived"));
}

BOOST_AUTO_TEST_SUITE_END()
