// ex07.cc
// vim: set sw=2:

#include <iostream>

using namespace std;

int
main()
{
  double V[3] = { 2.0, 0.0, 9.0 };
  double W[3] = { 12.0, 13.0, 14.0 };
  for (auto I = 0; I <= 3; ++I) {
    cout << V[I] << endl;
    cout << W[I] << endl;
  }
  return 0;
}

