// ex03_fixed.cc
// vim: set sw=2:
//
// Missing continue.
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
  vector<int> V = { 1, 2, 3 };
  for (auto val : V) {
    if (val == 2) {
      cout << val << endl;
      continue;
    }
    cout << (val * 10) << endl;
  }
  return 0;
}

