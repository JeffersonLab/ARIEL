//
//  Propagate the generated tracks through the simple detector.
//  No scattering, energy loss, decays etc.
//  Only follow each track until it completes its first outgoing arc or until it
//  leaves the detector, whichever comes first.
//
//  The algorithm is not very smart.  It assumes that both the point of origin
//  of the track, and its point of closest approach to the z axis, are inside
//  the innermost tracker shell; if they are not, then the algorithm may miss
//  hits or mis-order hits.

#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/Geometry/IntersectionFinder.h"
#include "toyExperiment/MCDataProducts/GenParticleCollection.h"
#include "toyExperiment/MCDataProducts/IntersectionCollection.h"
#include "toyExperiment/MCDataProducts/MCRunSummary.h"
#include "toyExperiment/PDT/PDT.h"
#include "toyExperiment/RecoDataProducts/DetectorStatus.h"
#include "toyExperiment/RecoDataProducts/Helix.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include <cmath>
#include <iostream>

namespace tex {

  class DetectorSimulation : public art::EDProducer {

  public:
    explicit DetectorSimulation(fhicl::ParameterSet const& pset);

    void beginRun(art::Run& run) override;
    void endRun(art::Run& run) override;
    void produce(art::Event& event) override;

  private:
    // The input tag the module that created the GenParticles.
    art::InputTag _gensTag;

    // The instance names of the TrkHitColleciton data products.
    // This is a more convenient form than that provided by TrackerComponent.
    std::vector<std::string> _instanceNames;

    // Handle to the geometry service; handle is defined at c'tor-time but
    // the contents may change on run and subrun boundaries.
    art::ServiceHandle<Geometry> _geom;

    // The particle data table; defined at c'tor-time.
    art::ServiceHandle<PDT> _pdt;

    // Keep some running statistics about the run to date.
    MCRunSummary _runSummary;
  };

}

tex::DetectorSimulation::DetectorSimulation(fhicl::ParameterSet const& pset)
  : EDProducer{pset}
  , _gensTag(pset.get<std::string>("genParticleTag"))
  , _instanceNames()
  , _geom(art::ServiceHandle<Geometry>())
  , _pdt()
  , _runSummary()
{

  // This module produces one data product for each element of the
  // TrackComponent enum.
  for (auto const& component : TrackerComponent::knownNames()) {
    produces<IntersectionCollection>(component.second);
    _instanceNames.push_back(component.second);
  }

  // Data products that will be added to the run object.
  produces<DetectorStatus, art::InRun>();
  produces<MCRunSummary, art::InRun>();
}

void
tex::DetectorSimulation::produce(art::Event& event)
{

  // Fetch input data product.
  art::Handle<GenParticleCollection> gensHandle;
  event.getByLabel(_gensTag, gensHandle);
  GenParticleCollection const& gens(*gensHandle);

  // Create empty output data products.
  std::vector<std::unique_ptr<IntersectionCollection>> products;
  for (size_t i = 0; i < _instanceNames.size(); ++i) {
    products.emplace_back(new IntersectionCollection);
  }

  // Access geometry information.
  Tracker const& tracker(_geom->tracker());
  double bz(_geom->bz());

  // Counters for the MCRunSummary
  size_t nAlive(0);
  size_t nGood(0);

  for (auto const& gen : gens) {

    // Index of this particle within the GenParticleCollection.
    int genIndex = &gen - &gens.front();

    // Skip particles that were decayed in the generator.
    if (!gen.hasChildren()) {

      ++nAlive;

      double q = _pdt->getById(gen.pdgId()).charge();
      Helix helix(gen.position(), gen.momentum().vect(), q, bz);

      // Another counter for the MCRunSummary
      size_t nHits(0);

      for (auto const& shell : tracker.shells()) {

        IntersectionFinder intersection(shell, helix);

        if (intersection.hasIntersection()) {

          auto product = products.at(shell.trackerComponent()).get();
          product->emplace_back(shell.id(),
                                art::Ptr<GenParticle>(gensHandle, genIndex),
                                intersection.position());
          ++nHits;

        } // end hasIntersection

      } // end loop over tracker shells

      if (nHits > 5) {
        ++nGood;
      }

    } // end test on alive
  }   // end loop over Genparticles

  _runSummary.increment(nAlive, nGood);

  // Add the output data products to the event.
  auto name = _instanceNames.begin();
  for (auto& product : products) {
    event.put(std::move(product), *(name++));
  }
}

void
tex::DetectorSimulation::beginRun(art::Run& run)
{

  // Clear the per run statistics.
  _runSummary.clear();

  // Build the detector status object.
  Tracker const& tracker(_geom->tracker());
  std::unique_ptr<DetectorStatus> status(new DetectorStatus(tracker.nShells()));
  for (auto const& shell : tracker.shells()) {
    double voltage =
      (shell.trackerComponent() == TrackerComponent::inner) ? 200. : 1500.;
    status->addRecord(DetectorStatusRecord(true, voltage));
  }

  run.put(std::move(status));
}

void
tex::DetectorSimulation::endRun(art::Run& run)
{

  // Make a copy of the per run statistics and add them to the event.
  std::unique_ptr<MCRunSummary> summary(new MCRunSummary(_runSummary));
  run.put(std::move(summary));

  std::cout << "MC Run Summary: " << _runSummary << std::endl;
}

std::ostream&
tex::operator<<(std::ostream& ost, const MCRunSummary& sum)
{
  ost << "MC Run Summary: number alive: " << sum.nAlive()
      << " number good: " << sum.nGood();
  return ost;
}

DEFINE_ART_MODULE(tex::DetectorSimulation)
