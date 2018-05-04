#include "cetlib/sha1.h"
#include <cstdlib>
#include <string>

using cet::sha1;

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

int
main()
{
  typedef sha1::digest_t digest_t;

  {
    sha1 d;
    d.digest();
  }

  {
    sha1 d1("hi");
    digest_t r1 = d1.digest();

    sha1 d2;
    d2 << "hi";
    digest_t r2 = d2.digest();

    sha1 d3('h');
    d3 << 'i';
    digest_t r3 = d3.digest();

    sha1 d4;
    d4 << 'h' << 'i';
    digest_t r4 = d4.digest();

    ensure(1, r1 == r2);
    ensure(2, r1 == r3);
    ensure(3, r1 == r4);
  }

  return 0;

} // main()
