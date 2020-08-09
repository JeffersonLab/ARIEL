//
// Some helper functions for sqrtOrThrow.
//

#include "toyExperiment/Utilities/sqrtOrThrow.h"

#include <stdexcept>
#include <sstream>

void tex::throwHelperForSqrtOrThrow( double val, double epsilon){
  std::ostringstream os;
  os << "sqrtOrThrow range error. Inputs " << val << " " << epsilon;
  throw std::range_error(os.str());
}

void tex::throwHelperForSqrtOrThrow( float val, float epsilon){
  std::ostringstream os;
  os << "sqrtOrThrow range error. Inputs " << val << " " << epsilon;
  throw std::range_error(os.str());
}
