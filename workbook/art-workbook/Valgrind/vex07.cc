// vex07.cc
// vim: set sw=2:
//
// Illegal read.
//

#include <iostream>

using namespace std;

int
main()
{
  int* p = nullptr;
  cout << *p << endl;
}

// Local Variables:
// mode: c++
// End:
