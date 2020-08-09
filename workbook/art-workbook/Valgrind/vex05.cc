// vex05.cc
// vim: set sw=2:
//
// A more complicated leak.
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
  }
  Track()
    : begin_(new Point)
    , middle_(new Point)
    , end_(new Point)
  {
  }
};

class Particle {
private:
  Track* track1_;
public:
  ~Particle()
  {
  }
  Particle()
    : track1_(new Track)
  {
  }
};

int
main()
{
  auto p = new Particle;
  delete p;
}

// Local Variables:
// mode: c++
// End:
