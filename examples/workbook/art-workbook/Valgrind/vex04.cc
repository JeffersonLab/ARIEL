// vex04.cc
// vim: set sw=2:
//
// Yet more object leaking practice.
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
  delete p;
}

// Local Variables:
// mode: c++
// End:
