#ifndef critic_test_art_RunTimeConsumes_h
#define critic_test_art_RunTimeConsumes_h

#include "art/Framework/Core/ConsumesCollector.h"
#include "canvas/Persistency/Provenance/BranchType.h"

#include <string>

namespace art::test {
  template <typename T>
  void
  run_time_consumes(ConsumesCollector& collector,
                    BranchType const bt,
                    std::string const& instance = {})
  {
    switch (bt) {
      case art::InEvent:
        collector.consumes<T>(instance);
        break;
      case art::InSubRun:
        collector.consumes<T, art::InSubRun>(instance);
        break;
      case art::InRun:
        collector.consumes<T, art::InRun>(instance);
        break;
      default:
        throw art::Exception(art::errors::LogicError)
          << "Unknown branch type " << bt << ".\n";
    }
  }
}

#endif /* critic_test_art_RunTimeConsumes_h */

// Local Variables:
// mode: c++
// End:
