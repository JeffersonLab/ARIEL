//
//  Generate a very simple event; add it to the event as a data product of type
//  GenParticleCollection.
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"
#include "toyExperiment/PDT/PDT.h"
#include "toyExperiment/Utilities/Decay2Body.h"
#include "toyExperiment/Utilities/RandomUnitSphere.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoissonQ.h"

#include <cmath>
#include <iostream>
#include <memory>

namespace tex {

  class EventGenerator : public art::EDProducer {

  public:
    explicit EventGenerator(fhicl::ParameterSet const& pset);
    void produce(art::Event& event) override;

  private:
    // Particle data table.
    art::ServiceHandle<PDT> _pdt;

    // Configuration information.
    int _seed;
    double _pmin;
    double _pmax;
    double _nExtra;
    double _pminExtra;
    double _pmaxExtra;

    // Particle masses.
    double _mpi;
    double _mka;
    double _mphi;

    // Random number engines and distributions.
    art::RandomNumberGenerator::base_engine_t& _engine;
    CLHEP::RandFlat _flat;
    CLHEP::RandPoissonQ _poisson;
    RandomUnitSphere _unitSphere;

    // Helper functions.
    void generateBG(int nExtra, GenParticleCollection& gens);
    void generateSignal(GenParticleCollection& gens, art::Event&);
  };

}

tex::EventGenerator::EventGenerator(fhicl::ParameterSet const& pset)
  : EDProducer{pset}
  , _pdt()
  ,

  _seed(pset.get<int>("seed"))
  , _pmin(pset.get<fhicl::ParameterSet>("signal").get<double>("pmin"))
  , _pmax(pset.get<fhicl::ParameterSet>("signal").get<double>("pmax"))
  ,

  _nExtra(pset.get<fhicl::ParameterSet>("background").get<double>("n"))
  , _pminExtra(pset.get<fhicl::ParameterSet>("background").get<double>("pmin"))
  , _pmaxExtra(pset.get<fhicl::ParameterSet>("background").get<double>("pmax"))
  ,

  _mpi(_pdt->getById(PDGCode::pi_plus).mass())
  , _mka(_pdt->getById(PDGCode::K_plus).mass())
  , _mphi(_pdt->getById(PDGCode::phi).mass())
  ,

  _engine(createEngine(_seed))
  , _flat(_engine)
  , _poisson(_engine, _nExtra)
  , _unitSphere(_engine)
{
  produces<GenParticleCollection>();
}

namespace {

  // Number of signal tracks.
  const int nSignal = 3;

  // All particles will be produced at the origin.
  CLHEP::Hep3Vector origin(0., 0., 0);
}

void
tex::EventGenerator::produce(art::Event& event)
{

  // Number of extra tracks to generate in this event.
  int nExtra = _poisson.fire();

  // Create an empty output data product
  std::unique_ptr<GenParticleCollection> gens(new GenParticleCollection());
  gens->reserve(2 * nExtra + nSignal);

  // Fill the output data product with the event info.
  generateBG(nExtra, *gens);
  generateSignal(*gens, event);

  // Put the output collection into the event.
  event.put(std::move(gens));
}

void
tex::EventGenerator::generateBG(int n, tex::GenParticleCollection& gens)
{

  for (int i = 0; i < 2 * n; ++i) {

    // Generate the 4 momentum
    double p = _flat.fire(_pminExtra, _pmaxExtra);
    CLHEP::Hep3Vector mom = _unitSphere.fire(p);
    double e = std::sqrt(_mpi * _mpi + p * p);
    CLHEP::HepLorentzVector lmom(mom, e);

    // Even numbered particles are pi+; odd are pi-
    PDGCode::type id = (i % 2 == 0) ? PDGCode::pi_plus : PDGCode::pi_minus;

    // Put the particle on the output collection.
    gens.push_back(GenParticle(id, art::Ptr<GenParticle>(), origin, lmom));
  }
}

void
tex::EventGenerator::generateSignal(tex::GenParticleCollection& gens,
                                    art::Event& event)
{

  // Product Id of the data product to be created; needed for persistent
  // pointers.
  art::ProductID gensID(event.getProductID<GenParticleCollection>());

  // Generate the 4 momentum of a phi, ignoring its natural width.
  double p = _flat.fire(_pmin, _pmax);
  CLHEP::Hep3Vector mom = _unitSphere.fire(p);
  double e = std::sqrt(_mphi * _mphi + p * p);
  CLHEP::HepLorentzVector lmom(mom, e);

  // The particles will be added to the collection in given order; these are
  // indicies into the collection
  int idxPhi(gens.size());
  int idxKplus(idxPhi + 1);
  int idxKminus(idxPhi + 2);

  // Put the phi into the output collection; it is a primary particle that has
  // no parent.
  gens.push_back(
    GenParticle(PDGCode::phi, art::Ptr<GenParticle>(), origin, lmom));

  // Particles 1 and 2, have a parent, it is particle 0;
  art::Ptr<GenParticle> parent(gensID, idxPhi, event.productGetter(gensID));

  // Decay the phi and add its decay products to the output collection.
  Decay2Body decay(lmom, _mka, _mka, _unitSphere);
  gens.push_back(GenParticle(PDGCode::K_plus, parent, origin, decay.p1()));
  gens.push_back(GenParticle(PDGCode::K_minus, parent, origin, decay.p2()));

  // Tell particle 0 about its children.
  gens.at(idxPhi).addChild(
    art::Ptr<GenParticle>(gensID, idxKplus, event.productGetter(gensID)));
  gens.at(idxPhi).addChild(
    art::Ptr<GenParticle>(gensID, idxKminus, event.productGetter(gensID)));
}

DEFINE_ART_MODULE(tex::EventGenerator)
