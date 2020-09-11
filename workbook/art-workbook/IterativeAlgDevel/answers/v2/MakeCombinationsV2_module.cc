//
// Make combinations for oppositely charged pairs of particles.
//

#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/PDT/PDT.h"
#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"
#include "toyExperiment/RecoDataProducts/RecoTrk.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"
#include "art-workbook/ExDataProducts/CombinationCollection.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileService.h"

#include "TH1F.h"

#include <string>

namespace tex {

  class MakeCombinations : public art::EDProducer {

  public:

    explicit MakeCombinations(fhicl::ParameterSet const& pset);

    void beginJob();
    void beginRun(art::Run&);
    void produce(art::Event& event);

  private:

    art::InputTag fitsTag_;

    art::ServiceHandle<PDT>               pdt_;
    art::ServiceHandle<art::TFileService> tfs_;

    double bz_;
    double mka_;
    double mphi_;

    TH1F* hMass_;
    TH1F* hSigM_;
    TH1F* hPull_;
  };

}

tex::MakeCombinations::MakeCombinations(fhicl::ParameterSet const& pset):
  fitsTag_( pset.get<std::string>("fitterInputTag") ),
  pdt_(),
  tfs_(),
  bz_(),
  mka_(pdt_->getById(PDGCode::K_plus).mass()),
  mphi_(pdt_->getById(PDGCode::phi).mass()),
  hMass_(nullptr),
  hSigM_(nullptr),
  hPull_(nullptr){

  produces<CombinationCollection>();

}

void tex::MakeCombinations::beginJob(){
  hMass_ = tfs_->make<TH1F>("Mass", "Reconstructed mass;[MeV]",        100, 1010., 1030. );
  hSigM_ = tfs_->make<TH1F>("SigM", "Reconstructed sigma(Mass);[MeV]", 100,    0.,    5. );
  hPull_ = tfs_->make<TH1F>("Pull", "Mass (Reco-Gen)/sigma",           100,   -5.,    5. );
}

void tex::MakeCombinations::beginRun(art::Run&){
  bz_ = art::ServiceHandle<Geometry>()->bz();
}

void tex::MakeCombinations::produce(art::Event& event){

  // Get the fitted helices from the event.
  auto const fitsHandle = event.getValidHandle<FittedHelixDataCollection>(fitsTag_);
  FittedHelixDataCollection const& fits(*fitsHandle);

  std::unique_ptr<CombinationCollection> combos( new CombinationCollection());

  // Loop over all unique pairs of tracks.
  for ( size_t i=0; i+1<fits.size(); ++i ) {

    FittedHelix fit1 { fits[i], bz_ };
    RecoTrk trk1 { fit1.lorentzAtPoca(mka_), fit1.lorentzAtPocaCov(mka_) };

    for ( size_t j=i+1; j<fits.size(); ++j){

      FittedHelix fit2 { fits[j], bz_ };

      // Skip a pair of tracks if they have the same sign of electric charge.
      if ( std::abs(fit1.q()+fit2.q()) > 0.5 ) continue;

      RecoTrk trk2 { fit2.lorentzAtPoca(mka_), fit2.lorentzAtPocaCov(mka_) };

      // For the two track system, compute the invariant mass, the error
      // and the pull from the MC truth.
      RecoTrk trk3 = trk1 + trk2;
      double m     = trk3.momentum().mag();
      double sigm  = trk3.sigmaMass();
      double mpull = (sigm > 0 ) ? (( m-mphi_)/sigm) : -10.;

      hMass_->Fill(m);
      hSigM_->Fill(sigm);
      hPull_->Fill(mpull);

      combos->push_back( Combination(trk3,
                                     art::Ptr<FittedHelixData>(fitsHandle, i),
                                     art::Ptr<FittedHelixData>(fitsHandle, j) ) );

    } // end inner loop over tracks
  }   // end outer loop over tracks

  event.put( std::move(combos) );

} // end produce

DEFINE_ART_MODULE(tex::MakeCombinations)
