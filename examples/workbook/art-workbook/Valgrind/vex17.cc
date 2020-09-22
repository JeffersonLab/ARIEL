// vex17.cc
// vim: set sw=2:
//
// Show use of initialized memory in an if statement.
//

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

class Point {
private:
  float x_ = 3.0f;
  float y_;
  float z_ = 9.0f;
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
    cout << "x_: " << scientific << x_ << endl;
    cout << "y_: " << scientific << y_ << endl;
    cout << "z_: " << scientific << z_ << endl;
  }
};

int
main()
{
  Point p;
  if (fabsf(p.y() - 5.0f) < 1e-6f) {
    cout << "p.y() is equal (enough) to 5.0!" << endl;
  }
  return 0;
}

// Local Variables:
// mode: c++
// End:
