#ifndef cetlib_hypot_h
#define cetlib_hypot_h

// ======================================================================
//
// hypot: Checked and unchecked Euclidean planar hypotenuse calculations
//
// ======================================================================

#include <cmath>
#include <limits>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------

namespace cet {
  template <class T>
  std::enable_if_t<std::is_arithmetic<T>::value, T> hypot(T x, T y);

  template <class T>
  std::enable_if_t<std::is_arithmetic<T>::value, T> unchecked_hypot(T x, T y);

  template <class T>
  std::enable_if_t<std::is_arithmetic<T>::value, T> checked_hypot(T x, T y);
}

// ----------------------------------------------------------------------
// unchecked_hypot<>:

template <class T>
inline std::enable_if_t<std::is_arithmetic<T>::value, T>
cet::unchecked_hypot(T x, T y)
{
  return std::hypot(x, y);
}

// ----------------------------------------------------------------------
// checked_hypot<>:

template <class T>
std::enable_if_t<std::is_arithmetic<T>::value, T>
cet::checked_hypot(T x, T y)
{

  if (std::isinf(x) || std::isinf(y))
    return std::numeric_limits<T>::infinity();
  else if (std::isnan(x) || std::isnan(y))
    return std::numeric_limits<T>::quiet_NaN();
  else
    return unchecked_hypot(x, y);

} // checked_hypot<>(,)

// ----------------------------------------------------------------------
// hypot<>:

template <class T>
inline std::enable_if_t<std::is_arithmetic<T>::value, T>
cet::hypot(T x, T y)
{
  return checked_hypot(x, y);
}

  // ======================================================================

#endif /* cetlib_hypot_h */

// Local Variables:
// mode: c++
// End:
