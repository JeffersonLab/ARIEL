// vex13.cc
// vim: set sw=2:
//
// Illegal memory access in the heap.
//

#include <iostream>
#include <vector>

using namespace std;

int
main()
{
  auto V = new vector<int>({2, 5, 9});
  cout << "*V:" << endl;
  for (auto I = 0; I < 5; ++I) {
    cout << "(*V)["
         << I
         << "]: "
         << (*V)[I]
         << endl;
  }
  delete V;
  return 0;
}

// Local Variables:
// mode: c++
// End:
