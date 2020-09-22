// ex04.cc
// vim: set sw=2:
//
// Integer divide by zero.
//

#include <iostream>
#include <vector>

using namespace std;

void
not_called()
{
  vector<int> junk = { 0 };
  junk.size();
  junk.capacity();
  junk.max_size();
  junk.data();
  junk[0];
  junk.at(0);
}

int
main()
{
  vector<int> V = { 2, 0, 4 };
  for (auto val : V) {
    if (val != 0) {
      cout << (400 / val) << endl;
    }
  }
  return 0;
}

