// ======================================================================
//
// test getenv
//
// ======================================================================

#include "cetlib/getenv.h"

#include "cetlib_except/exception.h"
#include <cstdlib>
#include <string>

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

int
main()
{
  ensure(1, cet::getenv("USER").size() > 0);

  try {
    std::string s = cet::getenv("XYZZY_PLUGH");
    ensure(11, false);
  }
  catch (cet::exception const& e) {
    ensure(12, e.category() == "getenv");
  }

  return 0;

} // main()
