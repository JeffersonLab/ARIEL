// vex16.cc
// vim: set sw=2:
//
// Show use of unitialized value returned by a member function.
//

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

class Point {
private:
  float x_ = 3.0;
  float y_;
  float z_ = 9.0;
public:
  Point() = default;
  float
  y() const
  {
    return y_;
  }
  void
  y(float const val)
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
  Point p;
  cout << "p:" << endl;
  p.print();
  if (fabs(p.y() - 5.0) < 1e-6) {
    cout << "p.y() is equal (enough) to 5.0!" << endl;
  }
  return 0;
}

// Local Variables:
// mode: c++
// End:
