#ifndef critic_test_art_product_aggregation_physics_workflow_TrackEfficiency_h
#define critic_test_art_product_aggregation_physics_workflow_TrackEfficiency_h

#include "canvas/Persistency/Provenance/IDNumber.h"

namespace arttest {

  class TrackEfficiency {
  public:
    explicit TrackEfficiency() = default;
    explicit TrackEfficiency(unsigned const num, unsigned const denom)
      : num_{num}, denom_{denom}
    {}

    auto
    efficiency() const
    {
      return static_cast<double>(num_) / denom_;
    }

    auto
    num() const
    {
      return num_;
    }
    auto
    denom() const
    {
      return denom_;
    }

    void
    aggregate(TrackEfficiency const& other)
    {
      num_ += other.num_;
      denom_ += other.denom_;
    }

  private:
    unsigned num_;
    unsigned denom_;
  };

} // namespace arttest

#endif /* critic_test_art_product_aggregation_physics_workflow_TrackEfficiency_h \
        */

// Local variables:
// mode: c++
// End:
