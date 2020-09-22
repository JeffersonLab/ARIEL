#include "art-workbook/IterativeAlgDevel/detail/FittedHelixDataFactory.h"
#include "art-workbook/IterativeAlgDevel/detail/ParticleInfoCollection.h"

#include "cetlib/pow.h"

#include "toyExperiment/Utilities/Decay2Body.h"
#include "toyExperiment/Utilities/RandomUnitSphere.h"
#include "toyExperiment/RecoDataProducts/Helix.h"
#include "toyExperiment/RecoDataProducts/FittedHelixData.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <cmath>
#include <iostream>

namespace {

  using namespace helper;
  using namespace helper::detail;
  using namespace tex;

  ParticleInfoCollection const pdt_ { "databaseFiles/pdt.fcl" };
  double const mka_  { pdt_.getById( PDGCode::K_plus ).mass() };
  double const mphi_ { pdt_.getById( PDGCode::phi    ).mass() };

  double const b_    {1.5};
  double const pmin_ {0.};
  double const pmax_ {2000.};
  CLHEP::Hep3Vector const origin_ {0.,0.,0};

  CLHEP::HepJamesRandom engine_; // use default seed
  CLHEP::RandFlat   flat_      { engine_ };
  RandomUnitSphere  unitSphere_{ engine_ };
  CLHEP::RandGaussQ gauss_     { engine_ };

  using ToyParticleCollection = std::vector<ToyParticle>;

  CLHEP::HepLorentzVector phiMomentum()
  {
    double const p              = flat_.fire( pmin_, pmax_ );
    CLHEP::Hep3Vector const mom = unitSphere_.fire(p);
    double const e              = std::sqrt( mphi_*mphi_ + p*p );
    return CLHEP::HepLorentzVector{ mom, e };
  }

}

//=====================================================================
FittedHelixDataCollection
helper::FittedHelixDataFactory::make(std::size_t const nneg, std::size_t const npos)
{

  auto const genParticles = FittedHelixDataFactory::generateParticles_(nneg,npos);
  return FittedHelixDataFactory::makeFitData_(genParticles);
}

//=====================================================================
ToyParticleCollection
helper::FittedHelixDataFactory::generateParticles_(std::size_t const npos,
                                                   std::size_t const nneg)
{

  std::size_t const n = std::max(npos,nneg);

  ToyParticleCollection result;
  result.reserve(2*n);

  for ( std::size_t ipos=0, ineg=0 ; ipos!=n || ineg!=n ; ++ipos, ++ineg ) {
    Decay2Body const decay{ phiMomentum(), mka_, mka_, unitSphere_ };
    if ( ipos < npos ) result.emplace_back( PDGCode::K_plus,  origin_, decay.p1() );
    if ( ineg < nneg ) result.emplace_back( PDGCode::K_minus, origin_, decay.p2() );
  }

  return result;
}

//=====================================================================
FittedHelixDataCollection
helper::FittedHelixDataFactory::makeFitData_(std::vector<ToyParticle> const& genParticles)
{
  // Using using did not work?  Why?
  enum local_type { icu=Helix::icu, iphi0=Helix::iphi0, id0   = Helix::id0,
                    ict=Helix::ict, iz0=Helix::iz0 };

  FittedHelixDataCollection result;
  FittedHelixData::hits_type hits(20); // create 20 hits with default-c'tored values

  for ( auto const& gen : genParticles ) {
    double const q  = pdt_.getById(gen.pdgId()).charge();
    if ( q == 0. ) continue;
    Helix trk ( gen.position(), gen.momentum().vect(), q, b_);

    CLHEP::HepVector par(5);
    par[icu]   = trk.cu();
    par[iphi0] = trk.phi0();
    par[id0]   = trk.d0();
    par[ict]   = trk.ct();
    par[iz0]   = trk.z0();

    // Initialize the matrix to all zeros.
    CLHEP::HepSymMatrix cov(5,0);

    // Wild guess at a the values of the diagonal elements.  Ignore the off-diagonals for now.
    double denom =  sqrt(hits.size());
    CLHEP::HepVector diag(5);
    diag[icu  ] = 0.04*std::abs(trk.cu())/denom;
    diag[iphi0] = 0.004/denom;
    diag[id0  ] = 0.05/denom;
    diag[ict  ] = 0.004/trk.sinTheta()/trk.sinTheta()/denom;
    diag[iz0  ] = 0.4/denom;

    // Form the covariance matrix and smear the tracks.
    for ( int i =icu; i<=iz0; ++i ){
      par[i]  += gauss_.fire()*diag[i];
      cov[i][i] = diag[i]*diag[i];
    }

    result.emplace_back( par, cov, hits );
  }

  return result;

}
