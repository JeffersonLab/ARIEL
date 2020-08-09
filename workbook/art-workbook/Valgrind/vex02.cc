// vex02.cc
// vim: set sw=2:
//
// Memory leak of an object of class type.
//

class Point {
private:
  float x_;
  float y_;
  float z_;
};

int
main()
{
  auto p = new Point;
  (void) p;
}

// Local Variables:
// mode: c++
// End:
