// ex02.cc
// vim: set sw=2:
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
  vector<int> V = { 0, 1, 2 };
  for (auto val : V) {
    switch (val) {
      case 0:
        cout << val << endl;
        break;
      case 1:
        cout << val << endl;
      case 2:
        cout << val << endl;
        break;
    }
  }
  return 0;
}

// Local Variables:
// mode: c++
// End:
