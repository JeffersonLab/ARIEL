#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"

namespace fhicl {
  class ParameterSet;
}

namespace art {
  class Event;
}

namespace critictest {

  class CriticFailEverything : public art::EDFilter {
  public:
    explicit CriticFailEverything(fhicl::ParameterSet const&);

  private:
    bool filter(art::Event&) override;
  };

  CriticFailEverything::CriticFailEverything(fhicl::ParameterSet const& ps)
    : EDFilter{ps}
  {}

  bool
  CriticFailEverything::filter(art::Event&)
  {
    return false;
  }
} // namespace critictest

using critictest::CriticFailEverything;
DEFINE_ART_MODULE(CriticFailEverything)
