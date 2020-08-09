// ex10.cc
// vim: set sw=2:

#include <iomanip>
#include <iostream>

using namespace std;

int
main()
{
  double V[3] = { 2.0, 0.0, 9.0 };
  double W[3] = { 12.0, 13.0, 14.0 };
  for (auto I = 0; I < 33; ++I) {
    W[I] = 64.0;
  }
  cout << "V[0]: " << scientific << V[0] << endl;
  cout << "W[0]: " << scientific << W[0] << endl;
  return 0;
}

