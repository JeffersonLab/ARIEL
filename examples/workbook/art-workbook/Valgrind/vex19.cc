// vex19.cc
// vim: set sw=2:
//
// Illegal write past end of bss.

#include <iostream>

using namespace std;

int I = 4;

int
main()
{
  int* p = &I;
  *p = 15;
  for (auto J = 0; J < 275; ++J) {
    p[J] = 0xFF;
  }
}

// Local Variables:
// mode: c++
// End:
