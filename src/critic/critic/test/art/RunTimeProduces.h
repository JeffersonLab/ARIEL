#ifndef critic_test_art_RunTimeProduces_h
#define critic_test_art_RunTimeProduces_h

#include "art/Framework/Core/ProducesCollector.h"
#include "canvas/Persistency/Provenance/BranchType.h"

#include <string>

namespace art::test {
  template <typename T>
  void
  run_time_produces(art::ProducesCollector& collector,
                    art::BranchType const bt,
                    std::string const& instance = {})
  {
    switch (bt) {
      case art::InEvent:
        collector.produces<T>(instance);
        break;
      case art::InSubRun:
        collector.produces<T, art::InSubRun>(instance);
        break;
      case art::InRun:
        collector.produces<T, art::InRun>(instance);
        break;
      default:
        throw art::Exception(art::errors::LogicError)
          << "Unknown branch type " << bt << ".\n";
    }
  }
}

#endif /* critic_test_art_RunTimeProduces_h */

// Local Variables:
// mode: c++
// End:
