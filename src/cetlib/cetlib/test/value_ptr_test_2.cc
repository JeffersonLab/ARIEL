#include "cetlib/value_ptr.h"

#include <cassert>
#include <iostream>
#include <map>
#include <memory>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "cetlib/compiler_macros.h"

class simple {
  int i;

public:
  static int n_alive;
  static int n_born;

  simple() : i(0)
  {
    ++n_alive;
    ++n_born;
  }
  explicit simple(int j) : i(j)
  {
    ++n_alive;
    ++n_born;
  }

  simple(simple const& s) : i(s.i)
  {
    ++n_alive;
    ++n_born;
  }

  ~simple() { --n_alive; }

  bool
  operator==(simple const& o) const
  {
    return i == o.i;
  }
  bool
  isSame(simple const& o) const
  {
    return &o == this;
  }

}; // simple

int simple::n_alive = 0;
int simple::n_born = 0;

int
main()
{
  assert(simple::n_alive == 0);
  {
    cet::value_ptr<simple> a(new simple(10));
    assert(simple::n_born == 1);
    assert(simple::n_alive == 1);

    {
      cet::value_ptr<simple> b(a);
      assert(simple::n_born == 2);
      assert(simple::n_alive == 2);

      assert(*a == *b);
      assert(a->isSame(*b) == false);
    } // b destroyed
    assert(simple::n_born == 2);
    assert(simple::n_alive == 1);
  } // a destroyed, too
  assert(simple::n_born == 2);
  assert(simple::n_alive == 0);

  {
    std::unique_ptr<simple> c(new simple(11));
    std::unique_ptr<simple> d(new simple(11));
    simple* pc = c.get();
    simple* pd = d.get();
    assert(pc != 0);
    assert(pd != 0);

    cet::value_ptr<simple> e(c.release());
    assert(c.get() == 0);
    assert(*d == *e);
    assert(e.operator->() == pc);

    cet::value_ptr<simple> f;
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

    assert(simple::n_alive == 2);
    assert(simple::n_born == 4);

    std::map<cet::value_ptr<simple>, int> m;
    m[f] = 0;
#if GCC_IS_AT_LEAST(4, 8, 1) || defined(__ICC) || defined(__clang__)
    // Avoids a copy of f.
    assert(simple::n_born == 5);
#else
    assert(simple::n_born == 6);
#endif
  }
  assert(simple::n_alive == 0);
#if GCC_IS_AT_LEAST(4, 8, 1) || defined(__ICC) || defined(__clang__)
  assert(simple::n_born == 5);
#else
  assert(simple::n_born == 6);
#endif
}
