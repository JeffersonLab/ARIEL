// ex14.cc
// vim: set sw=2:

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
  AFloat af;
  auto ival = af.i;
  auto xval = af.x;
  cout << "af.i: " << ival << endl;
  cout << "af.x: " << scientific << xval << endl;
  cout << "fabsf(xval - 5.0f): " << scientific << fabsf(xval - 5.0f) << endl;
  if (fabsf(xval - 5.0f) < 1e-6f) {
    cout << "xval is equal (enough) to 5.0!" << endl;
  }
  return 0;
}

// Local Variables:
// mode: c++
// End:
