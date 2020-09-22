//
// Some statistics about an event. Not very complete ...
//

#include "art-workbook/SimpleDataProducts/EventSummary.h"

#include <ostream>

tex::EventSummary::EventSummary():
  nPositive_(0),
  nNegative_(0){
}

#ifndef __GCCXML__

void tex::EventSummary::increment( int q){
  if ( q > 0 ){
    ++nPositive_;
  } else{
    ++nNegative_;
  }
}

std::ostream& tex::operator<<(std::ostream& ost,
                         const tex::EventSummary& summary){
  ost << "( Event Summary: Tracks: Postive: "
      << summary.nPositive()
      << " Negative: "
      << summary.nNegative()
      << ")";

  return ost;
}

#endif // __GCCXML__
