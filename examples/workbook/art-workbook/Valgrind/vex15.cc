// vex15.cc
// vim: set sw=2:
//
// Uninitialized data member.
//

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

class AFloat {
public:
  int i;
  float x;
};

int
main()
{
  auto p = new AFloat;
  cout << "p->i: " << p->i << endl;
  cout << "p->x: " << scientific << p->x << endl;
  return 0;
}

// Local Variables:
// mode: c++
// End:
