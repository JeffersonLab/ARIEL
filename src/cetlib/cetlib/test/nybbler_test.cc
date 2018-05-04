#include "cetlib/nybbler.h"
#include <cstdlib>
#include <string>

using cet::nybbler;

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
    nybbler n1;
    n1 << "abc";
    nybbler n2("abc");
    ensure(1, n1.as_hex() == n2.as_hex());
    ensure(2, n1.as_char() == n2.as_char());
  }

  {
    std::string s("xyz");
    nybbler n1(s);
    nybbler n2(n1.as_hex());
    ensure(3, n2.as_char() == s);
  }

  {
    std::string s("494A");
    nybbler n1(s);
    nybbler n2(n1.as_char());
    ensure(4, n1.as_char() == "IJ");
    ensure(5, n2.as_hex() == s);
  }

  return 0;
}
