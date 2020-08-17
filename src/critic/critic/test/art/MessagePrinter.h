#ifndef critic_test_art_MessagePrinter_h
#define critic_test_art_MessagePrinter_h

#include <ostream>
#include <string>

namespace fhicl {
  class ParameterSet;
}

namespace arttest {
  class MessagePrinter {
  public:
    MessagePrinter(fhicl::ParameterSet const&) {}
    void
    print(std::ostream& os, std::string const& msg)
    {
      os << msg;
    }
  };
} // namespace arttest

#endif /* critic_test_art_MessagePrinter_h */

// Local variables:
// mode: c++
// End:
