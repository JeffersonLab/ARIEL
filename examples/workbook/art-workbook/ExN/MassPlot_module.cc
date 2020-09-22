//
// Pull combinations out of the event and make TH1's, some TNtuples ...
//

#include "art-workbook/ExDataProducts/CombinationCollection.h"
#include "art-workbook/ExUtilities/PSetChecker.h"
#include "art-workbook/ExN/TrackCuts.h"

#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/Conditions/Conditions.h"
#include "toyExperiment/PDT/PDT.h"
#include "toyExperiment/RecoDataProducts/RecoTrk.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"
#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileService.h"

#include "TH1F.h"
#include "TNtuple.h"

#include "cetlib/exception.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <string>

namespace tex {

  class MassPlot : public art::EDAnalyzer {

  public:

    explicit MassPlot(fhicl::ParameterSet const& pset);

    void beginRun( art::Run   const& run   ) override;
    void analyze ( art::Event const& event ) override;

  private:

    art::InputTag combinerTag_;
    int           maxPrint_;
    TrackCuts     cuts_;

    art::ServiceHandle<Geometry>          geom_;
    art::ServiceHandle<Conditions>        conditions_;
    art::ServiceHandle<PDT>               pdt_;
    art::ServiceHandle<art::TFileService> tfs_;

    // PDG mass of the phi meson.
    double mphi_;

    // z component of the magnetic field; only deined after the first begin run.
    double bz_;

    int printCount_;

    TH1F* hNCombos_;
    TH1F* hNHits_;
    TH1F* hMinHits_;
    TH1F* hPTrack_;
    TH1F* hPMin_;
    TH1F* hMass_;
    TH1F* hSigM_;
    TH1F* hPull_;

    // Helper functions.
    void checkParameterSet( fhicl::ParameterSet const& pset );

  };

}

tex::MassPlot::MassPlot(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset),
  combinerTag_( pset.get<std::string>        ("combinerInputTag") ),
  maxPrint_(    pset.get<int>                ("maxPrint",0) ),
  cuts_(        pset.get<fhicl::ParameterSet>("cuts")),
  geom_(),
  conditions_(),
  pdt_(),
  tfs_( art::ServiceHandle<art::TFileService>() ),
  mphi_(pdt_->getById(PDGCode::phi).mass()),
  bz_(0.),
  printCount_(0),
  hNCombos_(nullptr),
  hNHits_(nullptr),
  hMinHits_(nullptr),
  hPTrack_(nullptr),
  hPMin_(nullptr),
  hMass_(nullptr),
  hSigM_(nullptr),
  hPull_(nullptr){

  checkParameterSet( pset );

}

void tex::MassPlot::beginRun( art::Run const& ){

  // The geometry is not defined until beginRun. So the next three variables cannot be
  // properly initialized earlier.
  bz_ = geom_->bz();

  // Get the number of layers in the tracking system.
  int nShells = geom_->tracker().nShells();

  // Set the limits for the hits per track histograms.
  int hitBins = nShells+5;

  hNCombos_ = tfs_->make<TH1F>("NCombos", "Number of combinations per event", 10,      0.,     10. );
  hNHits_   = tfs_->make<TH1F>("NHits",   "Number of hits Per Track",         hitBins, 0., hitBins );
  hMinHits_ = tfs_->make<TH1F>("MinHits", "Minimum Number of hits Per Track", hitBins, 0., hitBins );

  hPTrack_  = tfs_->make<TH1F>("PTrack",  "Momentum of each Track;[MeV]",                125, 0., 2500.);
  hPMin_    = tfs_->make<TH1F>("PMin",    "Minimum momentum of the two daughters;[MeV]", 125, 0., 2500.);

  hMass_ = tfs_->make<TH1F>("Mass", "Reconstructed mass;[MeV]",        100, 1010., 1030. );
  hSigM_ = tfs_->make<TH1F>("SigM", "Reconstructed sigma(Mass);[MeV]", 100,    0.,    5. );
  hPull_ = tfs_->make<TH1F>("Pull", "Mass (Reco-Gen)/sigma",           100,   -5.,    5. );

}

void tex::MassPlot::analyze( art::Event const& event){

  auto combos = event.getValidHandle<CombinationCollection>( combinerTag_ );

  hNCombos_->Fill( combos->size() );

  for ( auto const& combo : *combos ){

    FittedHelixData const& fit0(*combo.child(0));
    FittedHelixData const& fit1(*combo.child(1));

    int nhit0 = fit0.hits().size();
    int nhit1 = fit1.hits().size();
    hNHits_->Fill(nhit0);
    hNHits_->Fill(nhit1);

    int nhits = std::min( nhit0, nhit1 );
    hMinHits_->Fill(nhits);

    // By convention, this cut is disabled if minHitsPerTrack is negative.
    if ( nhits < cuts_.minHitsPerTrack ) continue;

    double p0 = fit0.helix().p(bz_);
    double p1 = fit1.helix().p(bz_);
    hPTrack_->Fill(p0);
    hPTrack_->Fill(p1);

    double pmin = std::min(p0,p1);
    hPMin_->Fill(pmin);

    if ( pmin < cuts_.pmin ) continue;

    RecoTrk trk  = combo.recoTrk();
    double m     = trk.momentum().mag();
    double sigm  = trk.sigmaMass();
    double mpull = (sigm > 0 ) ? (( m-mphi_)/sigm) : -10.;

    hMass_->Fill(m);
    hSigM_->Fill(sigm);
    hPull_->Fill(mpull);

  }

} // end produce

void tex::MassPlot::checkParameterSet( fhicl::ParameterSet const& pset ){

  std::vector<std::string> knownParameterNames = { "combinerInputTag", "maxPrint", "cuts" };
  PSetChecker checker( knownParameterNames, pset );
  if ( checker.bad() ){
    throw cet::exception("PSET") << checker.message("MassPlot") << "\n";
  }
}

DEFINE_ART_MODULE(tex::MassPlot)
