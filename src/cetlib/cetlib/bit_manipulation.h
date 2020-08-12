#ifndef cetlib_bit_manipulation_h
#define cetlib_bit_manipulation_h

// ======================================================================
//
// bit_manipulation: Compile-time bit manipulations
//
// ======================================================================

#include <cstddef>
#include <limits>
#include <type_traits>

namespace cet {

  /// struct bit_size<U>.
  template <class U, bool = std::is_unsigned<U>::value>
  struct bit_size;

  template <class U>
  struct bit_size<U, true> {
    static constexpr std::size_t value = std::numeric_limits<U>::digits;
  };

  /// struct bit_number<U, n>.
  template <class U,
            std::size_t n,
            bool = n<bit_size<U>::value> struct bit_number;

  template <class U, std::size_t n>
  struct bit_number<U, n, true> {
    static constexpr std::size_t value = U(1u) << n;
  };

  template <class U, std::size_t n>
  struct bit_number<U, n, false> {
    static constexpr std::size_t value = U(0u);
  };

  /// struct right_bits<U, n>.
  template <class U,
            std::size_t n,
            bool = std::is_unsigned<U>::value,
            bool = (n + 1) < bit_size<U>::value>
  struct right_bits;

  template <class U, std::size_t n>
  struct right_bits<U, n, true, true> {
    static constexpr U value = bit_number<U, n + 1>::value - static_cast<U>(1u);
  };

  template <class U, std::size_t n>
  struct right_bits<U, n, true, false> {
    static constexpr U value = ~0u;
  };

  // struct left_bits<U, n>.
  template <class U,
            std::size_t n,
            bool = std::is_unsigned<U>::value,
            bool = n <= bit_size<U>::value>
  struct left_bits;

  template <class U, std::size_t n>
  struct left_bits<U, n, true, true> {
  private:
    static constexpr U n_zeros = bit_size<U>::value - n;

  public:
    static constexpr U value = ~right_bits<U, n_zeros>::value;
  };

  template <class U, std::size_t n>
  struct left_bits<U, n, true, false> {
    static constexpr U value = U(-1);
  };

  // U circ_lshift<U>().
  template <class U>
  inline std::enable_if_t<std::is_unsigned<U>::value, U>
  circ_lshift(U X, U n)
  {
    constexpr std::size_t nbits = bit_size<U>::value;
    n %= nbits;
    return (n == 0) ? X : (X << n) | (X >> (nbits - n));
  }
}
#endif /* cetlib_bit_manipulation_h */

// Local Variables:
// mode: c++
// End:
