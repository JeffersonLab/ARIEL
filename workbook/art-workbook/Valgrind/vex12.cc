// vex12.cc
// vim: set sw=2:
//
// Example of a memory overwrite that memcheck cannot detect.
//

#include <iostream>

using namespace std;

class Point {
private:
  double x_;
  double y_;
  double z_;
public:
  Point()
    : x_(3.0)
    , y_(5.0)
    , z_(9.0)
  {
  }
  double
  y() const
  {
    return y_;
  }
  void
  y(double const val)
  {
    y_ = val;
  }
  void
  print() const
  {
    cout << "x_: " << x_ << endl;
    cout << "y_: " << y_ << endl;
    cout << "z_: " << z_ << endl;
  }
};

int
main()
{
  Point p1;
  Point p2;
  double a[] = { 42.0 };
  cout << "p1:" << endl;
  p1.print();
  cout << "-----" << endl;
  cout << "p2:" << endl;
  p2.print();
  cout << "-----" << endl;
  cout << "a:" << endl;
  for (auto val : a) {
    cout << val << endl;
  }
  for (auto I = 0; I < 5; ++I) {
    a[I] = 53.0;
  }
  cout << "-----" << endl;
  cout << "p1:" << endl;
  p1.print();
  cout << "-----" << endl;
  cout << "p2:" << endl;
  p2.print();
  return 0;
}

// Local Variables:
// mode: c++
// End:
