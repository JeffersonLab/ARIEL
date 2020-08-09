// ex06.cc
// vim: set sw=2:
//
// Demo of square root of a negative number.
//

#include <cmath>
#include <iostream>

using namespace std;

int
main()
{
  double val = -1.0;
  double tmp = sqrt(val);
  cout << tmp << endl;
  return 0;
}

