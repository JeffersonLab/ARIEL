// ex01.cc
// vim: set sw=2:
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Point {
private:
  float x_ = 3.0f;
  float y_ = 1.0f;
  float z_ = 4.0f;
public:
  Point();
  Point(float, float, float);
  void print() const;
};

Point::Point()
{
}

Point::Point(float x, float y, float z)
  : x_(x)
  , y_(y)
  , z_(z)
{
}

void
Point::print() const
{
  cout << "x_: " << x_ << endl;
  cout << "y_: " << y_ << endl;
  cout << "z_: " << z_ << endl;
}

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

void
printIntVector(vector<int> const& vec)
{
  for (auto val : vec) {
    cout << val << " ";
  }
  cout << endl;
}

int gI = 3;
Point gPt;
string gS = "Hello world.";

int
main()
{
  int M = 2;
  int* P = &M;
  (void) M;
  (void) P;
  Point pt1;
  pt1.print();
  vector<int> myVec = { 3, 1, 4 };
  for (auto I = 0; I < 3; ++I) {
    printIntVector(myVec);
  }
  return 0;
}

// Local Variables:
// mode: c++
// End:
