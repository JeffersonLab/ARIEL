#ifndef critic_test_art_product_aggregation_physics_workflow_CalibConstants_h
#define critic_test_art_product_aggregation_physics_workflow_CalibConstants_h

#include "canvas/Persistency/Provenance/IDNumber.h"

namespace arttest {

  class CalibConstants {
  public:
    explicit CalibConstants() = default;
    explicit CalibConstants(art::SubRunNumber_t const sr) : subrun_{sr} {}
    auto
    subrun() const
    {
      return subrun_;
    }
    void
    aggregate(CalibConstants const&)
    {}

  private:
    art::SubRunNumber_t subrun_;
  };

} // namespace arttest

#endif /* critic_test_art_product_aggregation_physics_workflow_CalibConstants_h \
        */

// Local variables:
// mode: c++
// End:
