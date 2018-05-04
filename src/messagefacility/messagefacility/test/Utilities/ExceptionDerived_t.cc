
#include "cetlib_except/exception.h"

#include <iomanip>
#include <iostream>
#include <string>

struct Thing : public cet::exception {
  Thing(const std::string& msg) : exception("Thing", msg) {}
};

std::ostream&
operator<<(std::ostream& os, const Thing& t)
{
  os << "Thing(" << t.explain_self() << ")";
  return os;
}

void
func3()
{
  throw Thing("Data Corrupt") << " Low level error" << std::endl;
}

void
func2()
{
  func3();
}

void
func1()
{
  try {
    func2();
  }
  catch (cet::exception& e) {
    throw cet::exception("InfiniteLoop", "In func2", e) << "Gave up";
  }
}

int
main()
{
  try {
    func1();
  }
  catch (cet::exception& e) {
    std::cerr << "*** main caught cet::exception, output is ***\n"
              << "(" << e.explain_self() << ")"
              << "*** After exception output ***" << std::endl;

    std::cerr << "\nCategory name list:\n";

#if 0
    if(e.explain_self() != answer) {
      std::cerr << "not right answer\n(" << answer << ")\n"
           << std::endl;
      abort();
    }
#endif
  }
  return 0;
}
