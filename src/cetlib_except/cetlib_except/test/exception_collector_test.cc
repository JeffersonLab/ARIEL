#include "cetlib_except/exception_collector.h"
#include <cstdlib>

using cet::exception_collector;

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

int calls_left = 2;
void
f()
{
  --calls_left;
  throw 1;
}
void
g()
{
  --calls_left;
  throw 2;
}

int
main()
{
  exception_collector ec;
  ensure(1, !ec.has_thrown());
  ensure(2, calls_left == 2);

  try {
    ec.call(f);
    ensure(11, calls_left == 1);
    ensure(12, ec.has_thrown());

    ec.call(g);
    ensure(13, calls_left == 0);
    ensure(14, ec.has_thrown());

    ec.rethrow();
    ensure(15, false);
  }
  catch (...) {
    ensure(16, true);
    ensure(17, !ec.has_thrown());
  }

  return 0;

} // main()
