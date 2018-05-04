#include "cetlib/value_ptr.h"

#include <cassert>
#include <memory>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

class simple_base {
  int i;

  simple_base(simple_base const& s) : i(s.i)
  {
    ++n_alive;
    ++n_born;
  }

public:
  static int n_alive;
  static int n_born;

  simple_base() : i(0)
  {
    ++n_alive;
    ++n_born;
  }
  explicit simple_base(int j) : i(j)
  {
    ++n_alive;
    ++n_born;
  }

  virtual simple_base*
  clone() const
  {
    return new simple_base(*this);
  }

  virtual ~simple_base() { --n_alive; }

  bool
  operator==(simple_base const& o) const
  {
    return i == o.i;
  }
  bool
  isSame(simple_base const& o) const
  {
    return &o == this;
  }

}; // simple_base

int simple_base::n_alive = 0;
int simple_base::n_born = 0;

int
main()
{
  assert(simple_base::n_alive == 0);
  {
    cet::value_ptr<simple_base> a(new simple_base(10));
    assert(simple_base::n_alive == 1);
    cet::value_ptr<simple_base> b(a);
    assert(simple_base::n_alive == 2);

    assert(*a == *b);
    assert(a->isSame(*b) == false);
  } // a and b destroyed
  assert(simple_base::n_alive == 0);

  {
    std::unique_ptr<simple_base> c(new simple_base(11));
    std::unique_ptr<simple_base> d(new simple_base(11));
    simple_base* pc = c.get();
    simple_base* pd = d.get();
    assert(pc != 0);
    assert(pd != 0);

    cet::value_ptr<simple_base> e(c.release());
    assert(c.get() == 0);
    assert(*d == *e);
    assert(e.operator->() == pc);

    cet::value_ptr<simple_base> f;
    if (f)
      assert(0);
    else {
    }
    f.reset(d.release());
    assert(d.get() == 0);
    assert(*e == *f);
    assert(f.operator->() == pd);
    if (f) {
    } else
      assert(0);

    assert(simple_base::n_alive == 2);
    assert(simple_base::n_born == 4);
  }
  assert(simple_base::n_alive == 0);
  assert(simple_base::n_born == 4);
}
