// vex06.cc
// vim: set sw=2:
//
// Master example of every possible kind of memory leak.
//

class Point {
private:
  float x_;
  float y_;
  float z_;
public:
  float*
  getSpecialY()
  {
    return &y_;
  }
};

class Track {
private:
  Point* begin_;
  Point* middle_;
  Point* end_;
  float* special_y_;
public:
  ~Track()
  {
    delete begin_;
    delete middle_;
    delete end_;
  }
  Track()
  {
    auto hit = new Point;
    special_y_ = hit->getSpecialY();
    hit = nullptr;
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

void
f()
{
  static Track* T = new Track;
  (void) T;
}

static Track* T = nullptr;

int
main()
{
  T = new Track;
  new Track;
  f();
}

// Local Variables:
// mode: c++
// End:
