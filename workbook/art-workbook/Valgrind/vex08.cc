// vex08.cc
// vim: set sw=2:
//
// Illegal write.

#include <iostream>

using namespace std;

int
main()
{
  int* p = nullptr;
  *p = 10;
  cout << *p << endl;
}

// Local Variables:
// mode: c++
// End:
