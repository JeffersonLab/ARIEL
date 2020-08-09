// ex04.cc
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
  vector<int> V = { 2, 0, 4 };
  for (auto val : V) {
    cout << (400 / val) << endl;
  }
  return 0;
}

