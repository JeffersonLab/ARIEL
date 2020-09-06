#ifndef critic_test_art_product_aggregation_physics_workflow_TaggedValue_h
#define critic_test_art_product_aggregation_physics_workflow_TaggedValue_h

#include "canvas/Utilities/InputTag.h"

namespace arttest {

  template <typename T>
  struct TaggedValue {
    explicit TaggedValue(std::string const& tag, T const value)
      : tag_{tag}, value_{value}
    {}
    art::InputTag const tag_;
    T const value_;
  };
}

#endif /* critic_test_art_product_aggregation_physics_workflow_TaggedValue_h   \
        */

// Local Variables:
// mode: c++
// End:
