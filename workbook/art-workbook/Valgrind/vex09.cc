// vex09.cc
// vim: set sw=2:
//
// Read of deleted memory.
//

#include <iostream>

using namespace std;

int
main()
{
  int* p = new int(35);
  cout << *p << endl;
  delete p;
  cout << *p << endl;
}

// Local Variables:
// mode: c++
// End:
