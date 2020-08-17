#ifndef critic_test_art_product_aggregation_physics_workflow_Geometry_h
#define critic_test_art_product_aggregation_physics_workflow_Geometry_h

#include "canvas/Persistency/Provenance/IDNumber.h"

namespace arttest {

  class Geometry {
  public:
    explicit Geometry() = default;
    explicit Geometry(art::RunNumber_t const r) : run_{r} {}
    auto
    run() const
    {
      return run_;
    }
    void
    aggregate(Geometry const&)
    {}

  private:
    art::RunNumber_t run_;
  };

} // namespace arttest

#endif /* critic_test_art_product_aggregation_physics_workflow_Geometry_h */

// Local variables:
// mode: c++
// End:
