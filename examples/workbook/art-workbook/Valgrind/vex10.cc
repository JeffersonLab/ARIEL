// vex10.cc
// vim: set sw=2:
//
// Write into deleted memory.
//

#include <iostream>

using namespace std;

int
main()
{
  int* p = new int(35);
  cout << *p << endl;
  *p = 10;
  cout << *p << endl;
  delete p;
  *p = 15;
}

// Local Variables:
// mode: c++
// End:
