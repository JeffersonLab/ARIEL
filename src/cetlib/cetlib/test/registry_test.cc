// ======================================================================
//
// test registry
//
// ======================================================================

#include "cetlib/registry.h"
#include <cstdlib>

using cet::registry;

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

int
main()
{
  int i;

  typedef registry<char const*, int> reg;
  ensure(1, reg::empty());
  ensure(2, reg::size() == 0);

  reg::put("one", 1);
  reg::put("two", 2);
  reg::put("three", 3);
  ensure(3, reg::size() == 3);
  reg::put("four", 4);
  reg::put("five", 5);
  reg::put("six", 6);
  reg::put("seven", 7);
  reg::put("eight", 8);
  reg::put("nine", 9);
  reg::put("zero", 0);

  ensure(11, !reg::empty());
  ensure(12, reg::size() == 10);
  ensure(13, reg::get("seven") == 7);
  ensure(14, reg::get("five") == 5);
  ensure(15, !reg::get("bleen", i));
  ensure(16, reg::get("two", i) && i == 2);

  try {
    reg::get("ten");
    ensure(21, false);
  }
  catch (cet::exception const& e) {
    ensure(22, e.category() == "cet::registry");
    std::string s = e.explain_self();
    ensure(23, s.find("not found in registry") != std::string::npos);
  }
  catch (...) {
    ensure(24, false);
  }

  return reg::get("zero");

} // main()
