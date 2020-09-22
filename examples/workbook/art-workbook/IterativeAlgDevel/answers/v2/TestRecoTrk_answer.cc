#include "art-workbook/IterativeAlgDevel/detail/CLHEP_helpers.h"

// Available for your use:
//
//    CLHEP::HepLorentzVector
//    CLHEP::HepSymMatrix

#include "toyExperiment/RecoDataProducts/RecoTrk.h"

using namespace tex;

int main()
{
  CLHEP::HepLorentzVector const vec1 { 1., 0., 0., 0.};
  CLHEP::HepLorentzVector const vec2 { 0., 1., 0., 0.};

  CLHEP::HepSymMatrix matrix { 4, 1 };
  RecoTrk const trk1 { vec1, matrix };
  RecoTrk const trk2 { vec2, matrix };

  // Check combined track momentum
  RecoTrk const trk3 = trk1 + trk2;
  if ( trk3.momentum() != CLHEP::HepLorentzVector{1.,1.,0.,0.} ) return 1;

  // Check combined track matrices
  CLHEP::HepSymMatrix matrix3{4}; // Cannot initialize diagonal
                                  // elements to any number other than
                                  // 0 or 1.
  for ( int const i : {0,1,2,3} )
    matrix3[i][i] = 2.;

  if ( trk3.momentumCov() != matrix3 ) return 2;

  return 0;
}
