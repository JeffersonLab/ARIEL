//
// Make combinations for oppositely charged pairs of particles.
//

#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/PDT/PDT.h"
#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"
#include "toyExperiment/RecoDataProducts/RecoTrk.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"

#include "art-workbook/ExDataProducts/CombinationCollection.h"
#include "art-workbook/IterativeAlgDevel/CombinationHelperImproved.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileService.h"

#include "TH1F.h"

#include <string>

using helper::CombinationData;

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

  std::vector<CombinationData> combsData = helper::makeCombinationsAlgImproved(fits, mka_, bz_);

  // Fill histograms
  for ( auto const & comb : combsData ) {
    RecoTrk const& trk = std::get<RecoTrk>(comb);
    double m     = trk.momentum().mag();
    double sigm  = trk.sigmaMass();
    double mpull = (sigm > 0 ) ? (( m-mphi_)/sigm) : -10.;

    hMass_->Fill(m);
    hSigM_->Fill(sigm);
    hPull_->Fill(mpull);
  }

  // Fill data product
  auto combProduct = std::make_unique<CombinationCollection>();
  for ( auto const & comb : combsData ) {
    RecoTrk     const& trk = std::get<0>(comb);
    std::size_t const  i   = std::get<1>(comb);
    std::size_t const  j   = std::get<2>(comb);
    combProduct->emplace_back( trk,
                               art::Ptr<FittedHelixData>(fitsHandle,i),
                               art::Ptr<FittedHelixData>(fitsHandle,j) );
  }

  event.put( std::move(combProduct) );

} // end produce

DEFINE_ART_MODULE(tex::MakeCombinations)
