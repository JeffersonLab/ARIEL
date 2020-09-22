//
// A crude reconstructed track class in cartesian representation.
//

#include "art-workbook/IterativeAlgDevel/RecoTrk.h"
#include "CLHEP/Matrix/Vector.h"

#include <array>

italgdev::RecoTrk::RecoTrk():
  _momentum(),
  _momCov(dimension){
}

#ifndef __GCCXML__

italgdev::RecoTrk::RecoTrk( CLHEP::HepLorentzVector const& momentum,
                            CLHEP::HepSymMatrix     const& momCov ):
  _momentum(momentum),
  _momCov(momCov){
}

italgdev::RecoTrk const& italgdev::RecoTrk::operator+=( RecoTrk const& rhs){
  _momentum += rhs._momentum;
  _momCov   += rhs._momCov;
  return *this;
}

double italgdev::RecoTrk::sigmaMass() const{

  double m = _momentum.mag();

  // Derivatives of the mass wrt the 4-vector components.
  std::array<double,dimension> dm{ - _momentum.x()/m, - _momentum.y()/m, - _momentum.z()/m, _momentum.e()/m };

  // Alias for typographic convenience.
  CLHEP::HepSymMatrix const& v(_momCov);

  // Error squared on the mass.
  double vm = dm[ipx]*dm[ipx]*v[ipx][ipx] +
              dm[ipy]*dm[ipy]*v[ipy][ipy] +
              dm[ipz]*dm[ipz]*v[ipz][ipz] +
              dm[ie ]*dm[ie ]*v[ie ][ie ] +
    2.* (  dm[ipx]*dm[ipy]*v[ipx][ipy] +
              dm[ipx]*dm[ipz]*v[ipx][ipz] +
              dm[ipx]*dm[ie ]*v[ipx][ie ] +
              dm[ipy]*dm[ipz]*v[ipy][ipz] +
              dm[ipy]*dm[ie ]*v[ipy][ie ] +
              dm[ipz]*dm[ie ]*v[ipz][ie ]
           );

  double sigma = ( vm > 0. ) ? sqrt(vm) : 0.;

  return sigma;
}

double italgdev::RecoTrk::sigmaP() const{

  double p = _momentum.vect().mag();

  // Derivatives of the mass wrt the 4-vector components.
  std::array<double,3> d{_momentum.x()/p, _momentum.y()/p, _momentum.z()/p};

  // Alias for typographic convenience.
  CLHEP::HepSymMatrix const& v(_momCov);

  // Error squared on the momentum.
  double vp = d[ipx]*d[ipx]*v[ipx][ipx] +
              d[ipy]*d[ipy]*v[ipy][ipy] +
              d[ipz]*d[ipz]*v[ipz][ipz] +
    2.* (  d[ipx]*d[ipy]*v[ipx][ipy] +
              d[ipx]*d[ipz]*v[ipx][ipz] +
              d[ipy]*d[ipz]*v[ipy][ipz]
           );

  double sigma = ( vp > 0. ) ? sqrt(vp) : 0.;

  return sigma;
}
#endif  // __GCCXML__
