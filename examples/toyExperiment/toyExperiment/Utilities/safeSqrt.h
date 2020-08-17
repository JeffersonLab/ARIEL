#ifndef Utilities_safeSqrt_h
#define Utilities_safeSqrt_h

//
//  Take the sqrt of its argument but protect against
//  roundoff error that can take the argument negative.
//
//  See also: sqrtOrThrow.hh
//

#include <cmath>

namespace tex {

  template <typename T>
  inline T
  safeSqrt(T x)
  {
    return (x > 0.) ? std::sqrt(x) : 0.;
  }

}
#endif /* Utilities_safeSqrt_h */
