// vex01.cc
// vim: set sw=2:
//
// Simplest example of a memory leak.
//

int
main()
{
  auto p = new int;
  (void) p;
}

// Local Variables:
// mode: c++
// End:
