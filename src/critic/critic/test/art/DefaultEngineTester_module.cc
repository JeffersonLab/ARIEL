#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/fwd.h"
#include "fhiclcpp/types/Atom.h"

#include "CLHEP/Random/Random.h"
#include "CLHEP/Random/RandomEngine.h"

#include <cassert>

namespace art::test {
  class DefaultEngineTester : public EDProducer {
  public:
    struct Config {};
    using Parameters = Table<Config>;
    explicit DefaultEngineTester(Parameters const&);

  private:
    void
    produce(Event&) override
    {}
  };

  DefaultEngineTester::DefaultEngineTester(Parameters const& p) : EDProducer{p}
  {
    assert(CLHEP::HepRandom::getTheEngine()->name() == "MixMaxRng");
    // Calling createEngine for the G4Engine type should set the
    // default CLHEP random-number engine to HepJamesRandom.
    auto& engine = createEngine(0, "G4Engine");
    assert(engine.name() == "HepJamesRandom");
    assert(CLHEP::HepRandom::getTheEngine()->name() == "HepJamesRandom");
  }
}

DEFINE_ART_MODULE(art::test::DefaultEngineTester)
