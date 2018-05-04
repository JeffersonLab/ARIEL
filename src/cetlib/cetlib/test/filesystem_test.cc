// ======================================================================
//
// test getenv
//
// ======================================================================

#include "cetlib/filesystem.h"

#include <cstdlib>

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

int
main()
{
  ensure(1, !cet::file_exists("xyxxy"));

  return 0;

} // main()
