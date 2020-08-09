// ex09.cc
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

class IntRefHolder {
private:
  int& ref_;
public:
  IntRefHolder(int& theRef)
    : ref_(theRef)
  {
  }
  int&
  getRef() const
  {
    return ref_;
  }
};

void
printRef(IntRefHolder const& theHolder)
{
  cout << "val: " << theHolder.getRef() << endl;
}

int
main()
{
  vector<int> V = { 2, 5, 23 };
  vector<int> W;
  for (auto I = V.begin(), E = (V.end() - 1); I != E; ++I) {
    IntRefHolder myHolder(*I);
    printRef(myHolder);
  }
  for (auto I = W.begin(), E = (W.end() - 1); I != E; ++I) {
    IntRefHolder myHolder(*I);
    printRef(myHolder);
  }
  return 0;
}

// Local Variables:
// mode: c++
// End:
