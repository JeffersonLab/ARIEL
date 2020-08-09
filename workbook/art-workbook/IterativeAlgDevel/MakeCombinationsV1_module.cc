//
// Make combinations for oppositely charged pairs of particles.
// This is the starting version of the module.
//

#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/PDT/PDT.h"
#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"

#include "TH1F.h"

#include <string>

using CLHEP::HepLorentzVector;
using CLHEP::HepSymMatrix;

namespace tex {

  class MakeCombinations : public art::EDProducer {

  public:

    explicit MakeCombinations(fhicl::ParameterSet const& pset);

    void beginJob();
    void beginRun(art::Run&);
    void produce(art::Event& event);

  private:

    art::InputTag fitsTag_;

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
  tfs_(),
  bz_(),
  mka_(493.677),
  mphi_(1019.455),
  hMass_(nullptr),
  hSigM_(nullptr),
  hPull_(nullptr){
}

void tex::MakeCombinations::beginJob() {
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

  // We will eventually create our new, empty data product here.


  // Loop over all unique pairs of tracks.
  // Pay careful attention to the form of the loop! We only want 'i' to go
  // up to i = (number of tracks) - 1. But we don't want wrong behavior
  // if (number of tracks) == 0, so we have to write the loop carefully.
  for ( size_t i = 0; i+1 < fits.size(); ++i ) {

    FittedHelix fit1 { fits[i], bz_ };

    // Get the parameters for track 1
    HepLorentzVector p1 { fit1.lorentzAtPoca(mka_) };
    HepSymMatrix     cov1 { fit1.lorentzAtPocaCov(mka_) };

    for ( size_t j=i+1; j<fits.size(); ++j){

      FittedHelix fit2 { fits[j], bz_ };

      // Skip a pair of tracks if the sum of their charges isn't zero.
      // We have to be careful about floating point math and exact
      // equality tests!
      if ( std::abs(fit1.q()+fit2.q()) > 0.5 ) continue;

      // Get the parameters for track 2
      HepLorentzVector p2 { fit2.lorentzAtPoca(mka_) };
      HepSymMatrix     cov2 { fit2.lorentzAtPocaCov(mka_) };

      // For the two track system, compute the invariant mass, the error
      // and the pull from the MC truth.

      // Momentum and covariance matrix are just sums of the two
      // inputs...
      HepLorentzVector p3 = p1 + p2;
      HepSymMatrix     cov3 = cov1 + cov2;

      double m     = p3.mag();

      // To get the squared error on the mass, we need:
      //  1) derivatives the the mass with respect to the 4-vector
      //  components.
      //
      //  2) to calculate the vector * matrix * vector product that
      //  yields the square of the uncertainty in the mass.
      //
      //  3) if the matrix was not ill-conditioned, the value from (2)
      //  will be positive; again, we have to be careful with the
      //  floating point math, even if the matrix was ill-conditioned.

      // Derivatives of the mass wrt the 4-vector components.
      double dm[4];
      dm[0] = -p3.x()/m;
      dm[1] = -p3.y()/m;
      dm[2] = -p3.z()/m;
      dm[3] =  p3.e()/m;

      // Error squared on the mass: vector * matrix * vector
      // calculation.
      double vm =
        dm[0]*dm[0]*cov3[0][0] +
        dm[1]*dm[1]*cov3[1][1] +
        dm[2]*dm[2]*cov3[2][2] +
        dm[3]*dm[3]*cov3[3][3] +
        2.* ( dm[0]*dm[1]*cov3[0][1] +
              dm[0]*dm[2]*cov3[0][2] +
              dm[0]*dm[3]*cov3[0][3] +
              dm[1]*dm[2]*cov3[1][2] +
              dm[1]*dm[3]*cov3[1][3] +
              dm[2]*dm[3]*cov3[2][3]
              );

      // Make sure to deal with ill-conditioned answer.
      double sigm = ( vm > 0. ) ? sqrt(vm) : 0.;
      double mpull = (sigm > 0 ) ? (( m-mphi_)/sigm) : -10.;

      hMass_->Fill(m);
      hSigM_->Fill(sigm);
      hPull_->Fill(mpull);

      // Here we will want to add our new combination to the data
      //  product we're accumulating.

    } // end inner loop over tracks
  }   // end outer loop over tracks

  // Here we should be putting a data product into the Event.

} // end produce

DEFINE_ART_MODULE(tex::MakeCombinations)
