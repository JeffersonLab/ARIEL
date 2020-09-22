// ex12.cc
// vim: set sw=2:
//
// Returning pointer to stack variable results
// in disaster at next function call.
//

#include <iomanip>
#include <iostream>

using namespace std;

class Point {
private:
  float x_ = 0.0f;
  float y_ = 0.0f;
  float z_ = 0.0f;
public:
  Point(float x, float y, float z);
  void print() const;
};

Point::Point(float x, float y, float z)
  : x_(x)
  , y_(y)
  , z_(z)
{
}

void
Point::print() const
{
  cout << "x_: " << scientific << x_ << endl;
  cout << "y_: " << scientific << y_ << endl;
  cout << "z_: " << scientific << z_ << endl;
}

int const*
getPtrToIntOnStack()
{
  int const I = 5;
  int const* p = &I;
  return p;
}

Point const*
getPtrToPointOnStack()
{
  const Point p(2.0f, 5.0f, 9.0f);
  Point const* pp = &p;
  return pp;
}

int I = 0;

void
overWriteStackVariablesWithFloats()
{
  float X[256];
  for (I = 0; I < 256; ++I) {
    X[I] = 3.1415926f;
  }
  (void) X;
}

void
overWriteStackVariablesWithInts()
{
  int X[2048];
  for (I = 0; I < 2048; ++I) {
    X[I] = 42;
  }
  (void) X;
}

int
main()
{
  cout << "-----" << endl;
  cout << "Calling getPtrToIntOnStack()!" << endl;
  int const* iosp = getPtrToIntOnStack();
  int const iosCopy = *iosp;
  // cout << "*iosp:" << endl;
  cout << *iosp << endl;
  cout << "iosCopy: " << endl;
  cout << iosCopy << endl;
  cout << "Calling overWriteStackVariablesWithInts()!" << endl;
  overWriteStackVariablesWithInts();
  // cout << "*iosp:" << endl;
  cout << *iosp << endl;
  cout << "iosCopy: " << endl;
  cout << iosCopy << endl;
  cout << "-----" << endl;
  cout << "Calling getPtrToPointOnStack()!" << endl;
  Point const* posp = getPtrToPointOnStack();
  // Note: Compiler chooses not to use the copy
  //       constructor here, so *posp is still valid!
  Point const posCopy = *posp;
  //cout << "posp:" << endl;
  posp->print();
  cout << "posCopy:" << endl;
  posCopy.print();
  cout << "Calling overWriteStackVariablesWithFloats()!" << endl;
  overWriteStackVariablesWithFloats();
  //cout << "posp:" << endl;
  posp->print();
  cout << "posCopy:" << endl;
  posCopy.print();
  return 0;
}

