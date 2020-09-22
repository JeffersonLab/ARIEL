// vex18.cc
// vim: set sw=2:
//
// Invalid delete.
//

int
main()
{
  int* p = new int(35);
  delete p;
  delete p;
}

// Local Variables:
// mode: c++
// End:
