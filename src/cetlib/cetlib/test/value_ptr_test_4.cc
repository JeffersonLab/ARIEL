#include "cetlib/value_ptr.h"

#include <cassert>
#include <memory>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

class base {
private:
  int i;

protected:
  base(base const& o) : i(o.i)
  {
    ++n_alive;
    ++n_born;
  }

public:
  static int n_alive;
  static int n_born;

  base() : i(0)
  {
    ++n_alive;
    ++n_born;
  }
  explicit base(int i) : i(i)
  {
    ++n_alive;
    ++n_born;
  }

  virtual base* clone() const = 0;

  virtual ~base() noexcept { --n_alive; }

  bool
  operator==(base const& o) const
  {
    return i == o.i;
  }
  bool
  isSame(base const& o) const
  {
    return &o == this;
  }

}; // base

int base::n_alive = 0;
int base::n_born = 0;

// ----------------------------------------------------------------------

class derived : public base {
private:
  int i;

protected:
  derived(derived const& s) : base(s), i(s.i)
  {
    ++n_alive;
    ++n_born;
  }

public:
  static int n_alive;
  static int n_born;
  static int n_cloned;

  derived() : i(0)
  {
    ++n_alive;
    ++n_born;
  }
  explicit derived(int i) : base(i), i(i)
  {
    ++n_alive;
    ++n_born;
  }

  derived*
  clone() const override
  {
    ++n_cloned;
    return new derived(*this);
  }

  ~derived() noexcept { --n_alive; }

  bool
  operator==(derived const& o) const
  {
    return this->base::operator==(o) && i == o.i;
  }
  bool
  isSame(derived const& o) const
  {
    return &o == this;
  }

}; // derived

int derived::n_alive = 0;
int derived::n_born = 0;
int derived::n_cloned = 0;

// ----------------------------------------------------------------------

int
main()
{
  assert(base::n_alive == 0);

  {
    cet::value_ptr<base> a(new derived(10));
    assert(base::n_alive == 1);
    assert(derived::n_alive == 1);
    assert(derived::n_cloned == 0);
    cet::value_ptr<base> b(a);
    assert(base::n_alive == 2);
    assert(derived::n_alive == 2);
    assert(derived::n_cloned == 1);

    assert(*a == *b);
    assert(a->isSame(*b) == false);
  } // a and b destroyed

  assert(base::n_alive == 0);
  assert(derived::n_alive == 0);

} // main()
