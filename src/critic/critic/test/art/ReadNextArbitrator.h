#ifndef critic_test_art_ReadNextArbitrator_h
#define critic_test_art_ReadNextArbitrator_h

#include "art/Utilities/ToolConfigTable.h"
#include "fhiclcpp/types/Atom.h"

namespace art::test {
  class ReadNextArbitrator {
  public:
    struct Config {
      fhicl::Atom<unsigned> threshold{fhicl::Name{"threshold"}, 5};
    };
    using Parameters = art::ToolConfigTable<Config>;
    ReadNextArbitrator(Parameters const& config)
      : threshold_{config().threshold()}
    {}
    auto
    threshold() const
    {
      return threshold_;
    }

  private:
    unsigned threshold_;
  };
} // namespace art::test

#endif /* critic_test_art_ReadNextArbitrator_h */

// Local variables:
// mode: c++
// End:
