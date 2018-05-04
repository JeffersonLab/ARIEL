#ifndef cetlib_test_TestPluginBase_h
#define cetlib_test_TestPluginBase_h

#include "cetlib/PluginTypeDeducer.h"
#include <string>

namespace cettest {
  class TestPluginBase;
}

namespace cet {
  template <>
  struct PluginTypeDeducer<cettest::TestPluginBase> {
    static std::string const value;
  };
}

class cettest::TestPluginBase {
public:
  TestPluginBase(std::string message);

  std::string const& message() const;

  virtual ~TestPluginBase() = default;

private:
  std::string const message_;
};
#endif /* cetlib_test_TestPluginBase_h */

// Local Variables:
// mode: c++
// End:
