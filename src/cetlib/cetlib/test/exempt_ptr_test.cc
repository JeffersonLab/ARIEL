#include "cetlib/exempt_ptr.h"
#include <cstdlib>

using cet::exempt_ptr;
using cet::make_exempt_ptr;

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

int
main()
{
  {
    exempt_ptr<int> p;
    ensure(11, !p);
    ensure(12, p == nullptr);
    ensure(13, nullptr == p);
    ensure(14, p == 0);
    ensure(15, 0 == p);
  }

  { // non-const => const
    int* p = new int(0);
    exempt_ptr<int const> ep(p);
  }

  {
    exempt_ptr<double const> p = make_exempt_ptr(new double(42.0));
    ensure(21, p != nullptr);
    ensure(22, nullptr != p);
    ensure(23, *p == 42.0);
  }

#if 0
  { // const => non-const (ought fail to compile)
    int const * p = new int(0);
    exempt_ptr<int> ep(p);
  }
#endif // 0

  {
    exempt_ptr<int> p(new int(16));
    ensure(31, p != nullptr);
    ensure(32, nullptr != p);
    ensure(33, *p == 16);

    exempt_ptr<int> q(p);
    ensure(34, p == q);
    ensure(35, *p == *q);

    p.reset(new int(0));
    return *p;
  }

} // main()
