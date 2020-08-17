#ifndef critic_test_art_ReadNext_h
#define critic_test_art_ReadNext_h

#include "fhiclcpp/ParameterSet.h"

namespace arttest {
  class ReadNextArbitrator {
  public:
    ReadNextArbitrator(fhicl::ParameterSet const& ps)
      : threshold_{ps.get<int>("threshold"), 5}
    {}
    auto
    threshold() const
    {
      return threshold_;
    }

  private:
    int threshold_;
  };
} // namespace arttest

#endif /* critic_test_art_ReadNext_h */

// Local variables:
// mode: c++
// End:
