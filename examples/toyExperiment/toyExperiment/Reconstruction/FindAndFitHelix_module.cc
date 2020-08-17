//
//  Crude mock-up of tracking finding and fitting.   A total cheat using MC
//  truth information for the track parameters and magic numbers for the
//  covariance matrix; then smear by the covariance matrix.
//
//  The goal is to have enough features present to allow us to develop the data
//  product and the classes that come after this in the workbook.
//

#include "toyExperiment/Conditions/Conditions.h"
#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/MCDataProducts/IntersectionCollection.h"
#include "toyExperiment/PDT/PDT.h"
#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"
#include "toyExperiment/RecoDataProducts/Helix.h"
#include "toyExperiment/RecoDataProducts/TrkHitCollection.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Persistency/Common/FindOne.h"

#include "CLHEP/Random/RandGaussQ.h"

#include <iostream>

namespace tex {

  class FindAndFitHelix : public art::EDProducer {

  public:
    explicit FindAndFitHelix(fhicl::ParameterSet const& pset);

    void produce(art::Event& event) override;

  private:
    art::InputTag _hitsTag;
    int _seed;
    size_t _minHits;
    int _maxPrint;

    art::ServiceHandle<Geometry> _geom;
    art::ServiceHandle<Conditions> _conditions;
    art::ServiceHandle<PDT> _pdt;

    // The random number engine that will be used by this data product.
    art::RandomNumberGenerator::base_engine_t& _engine;

    // Random number distributions.
    CLHEP::RandGaussQ _gauss;

    int _printCount
#ifdef __clang__
      [[gnu::unused]]
#endif
      ;

    typedef std::map<art::Ptr<GenParticle>, std::vector<art::Ptr<TrkHit>>>
      hitmap_type;

    void cheatPatternRecognition(art::Event const&, hitmap_type&);

    void parameterizedSmear(art::Ptr<GenParticle> const&,
                            std::vector<art::Ptr<TrkHit>> const&,
                            FittedHelixDataCollection&);
  };

}

tex::FindAndFitHelix::FindAndFitHelix(fhicl::ParameterSet const& pset)
  : EDProducer{pset}
  , _hitsTag(pset.get<std::string>("hitsTag"))
  , _seed(pset.get<int>("seed"))
  , _minHits(pset.get<size_t>("minHits"))
  , _maxPrint(pset.get<int>("maxPrint", 0))
  , _geom()
  , _pdt()
  , _engine(createEngine(_seed))
  , _gauss(_engine)
  , _printCount(0)
{

  if (_minHits < 4) {
    std::cout << "Warning: minimum hits is less than 4; setting it to 4."
              << std::endl;
    _minHits = 4;
  }

  produces<FittedHelixDataCollection>();
}

void
tex::FindAndFitHelix::produce(art::Event& event)
{

  // Create an empty output data product.
  std::unique_ptr<FittedHelixDataCollection> tracks(
    new FittedHelixDataCollection);

  // Fake pattern recognition: use MC truth to learn which hits are on which
  // tracks.
  hitmap_type hitsOnTracks;
  cheatPatternRecognition(event, hitsOnTracks);

  // For each track, do the fake fit.
  for (auto const& track : hitsOnTracks) {
    if (track.second.size() >= _minHits) {
      // Fake fit: parameterize the covariance matrix and smear by that matrix.
      parameterizedSmear(track.first, track.second, *tracks);
    }
  }

  // Add the fitted tracks to the event.
  event.put(std::move(tracks));
}

// Use the MC truth information to make a collection of hits on each generated
// track.
void
tex::FindAndFitHelix::cheatPatternRecognition(art::Event const& event,
                                              hitmap_type& hitsOnTracks)
{

  art::Handle<TrkHitCollection> hitsHandle;
  event.getByLabel(_hitsTag, hitsHandle);
  TrkHitCollection const& hits(*hitsHandle);

  // Navigator object to match hits with their truth information.
  art::FindOne<Intersection> fa(hitsHandle, event, _hitsTag);

  for (size_t i = 0; i < hits.size(); ++i) {
    Intersection const& truth(fa.at(i).ref());
    hitsOnTracks[truth.genTrack()].emplace_back(hitsHandle, i);
  }
}

void
tex::FindAndFitHelix::parameterizedSmear(
  art::Ptr<GenParticle> const& gen,
  std::vector<art::Ptr<TrkHit>> const& hits,
  FittedHelixDataCollection& fits)
{

  // Using using did not work?  Why?
  enum local_type {
    icu = Helix::icu,
    iphi0 = Helix::iphi0,
    id0 = Helix::id0,
    ict = Helix::ict,
    iz0 = Helix::iz0
  };

  double bz = _geom->bz();
  double q = _pdt->getById(gen->pdgId()).charge();
  Helix trk(gen->position(), gen->momentum().vect(), q, bz);

  CLHEP::HepVector par(5);
  par[icu] = trk.cu();
  par[iphi0] = trk.phi0();
  par[id0] = trk.d0();
  par[ict] = trk.ct();
  par[iz0] = trk.z0();

  // Initialize the matrix to all zeros.
  CLHEP::HepSymMatrix cov(5, 0);

  // Wild guess at a the values of the diagonal elements.  Ignore the
  // off-diagonals for now.
  double denom = sqrt(hits.size());
  CLHEP::HepVector diag(5);
  diag[icu] = 0.04 * std::abs(trk.cu()) / denom;
  diag[iphi0] = 0.004 / denom;
  diag[id0] = 0.05 / denom;
  diag[ict] = 0.004 / trk.sinTheta() / trk.sinTheta() / denom;
  diag[iz0] = 0.4 / denom;

  // Form the covariance matrix and smear the tracks.
  for (int i = icu; i <= iz0; ++i) {
    par[i] += _gauss.fire() * diag[i];
    cov[i][i] = diag[i] * diag[i];
  }

  fits.emplace_back(par, cov, hits);
}
DEFINE_ART_MODULE(tex::FindAndFitHelix)
