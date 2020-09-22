// vex03.cc
// vim: set sw=2:
//
// More complicated leak of an object of class type.
//

class Point {
private:
  float x_;
  float y_;
  float z_;
};

class Track {
private:
  Point* begin_;
  Point* middle_;
  Point* end_;
public:
  ~Track()
  {
    delete begin_;
    delete end_;
  }
  Track()
    : begin_(new Point)
    , middle_(new Point)
    , end_(new Point)
  {
  }
};

int
main()
{
  auto p = new Track;
  (void) p;
}

// Local Variables:
// mode: c++
// End:
