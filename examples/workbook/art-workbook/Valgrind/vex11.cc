// vex11.cc
// vim: set sw=2:
//
// Use of deleted memory through a member function.
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
  auto p = new Point;
  p->print();
  cout << "-----" << endl;
  cout << "fetched y_: " << p->y() << endl;
  cout << "-----" << endl;
  p->y(45.0);
  cout << "set y_: " << p->y() << endl;
  delete p;
  cout << "-----" << endl;
  cout << "broken fetched y_: " << p->y() << endl;
  cout << "-----" << endl;
  p->y(23.0);
  cout << "broken set y_: " << p->y() << endl;
  return 0;
}

// Local Variables:
// mode: c++
// End:
