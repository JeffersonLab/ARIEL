// ======================================================================
//
// test value_ptr
//
// ======================================================================

#define BOOST_TEST_MODULE (value_ptr test)
#include "cetlib/quiet_unit_test.hpp"
#include <ostream>
#include <string>

#include "cetlib/value_ptr.h"

namespace cet {
  std::ostream&
  boost_test_print_type(std::ostream& os, value_ptr<int> const p)
  {
    return os << p.get();
  }
}

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
  BOOST_TEST(!p);
  BOOST_TEST(p == nullptr);
  BOOST_TEST(nullptr == p);
}

BOOST_AUTO_TEST_CASE(basic_test)
{
  value_ptr<int> p(new int(16));
  BOOST_TEST(static_cast<bool>(p));
  BOOST_TEST(*p == 16);

  int* p_addr = p.get();
  value_ptr<int> q(p);
  BOOST_TEST(*p == *q);
  BOOST_TEST(p != q);
  BOOST_TEST(p_addr != q.get());

  p.reset(new int(0));
  BOOST_TEST(*p == 0);
  BOOST_TEST(p_addr != p.get());
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
  BOOST_TEST(bool(b));
  BOOST_TEST(b->who() == std::string("Base"));

  value_ptr<Base, default_copy<Base> // request slicing
            >
    d(new Derived);
  BOOST_TEST(bool(d));
  BOOST_TEST(d->who() == std::string("Derived"));
}

BOOST_AUTO_TEST_SUITE_END()
