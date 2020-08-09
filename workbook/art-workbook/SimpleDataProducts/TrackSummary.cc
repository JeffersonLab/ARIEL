//
// Some statistics about a track. Not very complete ...
//

#include "art-workbook/SimpleDataProducts/TrackSummary.h"

#include <ostream>

tex::TrackSummary::TrackSummary():
  nHitsInner_(0),
  nHitsOuter_(0){
}

#ifndef __GCCXML__

void tex::TrackSummary::incrementInner(){
  ++nHitsInner_;
}

void tex::TrackSummary::incrementOuter(){
  ++nHitsOuter_;
}

std::ostream& tex::operator<<(std::ostream& ost,
                         const tex::TrackSummary& summary){
  ost << "( Track Summary: Inner Hitss: "
      << summary.nHitsInner()
      << " Outer Hits: "
      << summary.nHitsOuter()
      << ")";

  return ost;
}

#endif // __GCCXML__
