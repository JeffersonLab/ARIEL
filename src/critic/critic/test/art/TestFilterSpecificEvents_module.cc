#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/container_algorithms.h"
#include "fhiclcpp/types/Sequence.h"

#include <vector>

using namespace fhicl;

namespace art::test {
  class TestFilterSpecificEvents;
}

class art::test::TestFilterSpecificEvents : public EDFilter {
public:
  struct Config {
    Sequence<unsigned> eventsToAccept{Name("eventsToAccept")};
  };
  using Parameters = EDFilter::Table<Config>;

  explicit TestFilterSpecificEvents(Parameters const& ps)
    : EDFilter{ps}, eventsToAccept_{ps().eventsToAccept()}
  {
    cet::sort_all(eventsToAccept_);
  }

private:
  bool
  filter(art::Event& e) override
  {
    return cet::binary_search_all(eventsToAccept_, e.event());
  }

  std::vector<unsigned> eventsToAccept_;
};

DEFINE_ART_MODULE(art::test::TestFilterSpecificEvents)
