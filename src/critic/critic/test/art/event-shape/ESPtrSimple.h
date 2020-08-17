#ifndef critic_test_art_event_shape_ESPtrSimple_h
#define critic_test_art_event_shape_ESPtrSimple_h

#include "art/test/TestObjects/ToyProducts.h"
#include "canvas/Persistency/Common/Ptr.h"

namespace arttest {
  struct ESPtrSimple;
}

struct arttest::ESPtrSimple {
  art::Ptr<Simple> p;
  void
  aggregate(ESPtrSimple const&) const
  {}
};

#endif /* critic_test_art_event_shape_ESPtrSimple_h */

// Local Variables:
// mode: c++
// End:
