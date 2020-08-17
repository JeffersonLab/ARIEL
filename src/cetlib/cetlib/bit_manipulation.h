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
  template <class U, bool = std::is_unsigned_v<U>>
  struct bit_size;

  template <class U>
  struct bit_size<U, true> {
    static constexpr std::size_t value = std::numeric_limits<U>::digits;
  };

  template <class U>
  constexpr std::size_t bit_size_v = bit_size<U>::value;

  /// struct bit_number<U, n>.
  template <class U, std::size_t n, bool = n<bit_size_v<U>> struct bit_number;

  template <class U, std::size_t n>
  struct bit_number<U, n, true> {
    static constexpr std::size_t value = U(1u) << n;
  };

  template <class U, std::size_t n>
  struct bit_number<U, n, false> {
    static constexpr std::size_t value = U(0u);
  };

  template <class U, std::size_t n>
  constexpr std::size_t bit_number_v = bit_number<U, n>::value;

  /// struct right_bits<U, n>.
  template <class U,
            std::size_t n,
            bool = std::is_unsigned_v<U>,
            bool = (n + 1) < bit_size_v<U>>
  struct right_bits;

  template <class U, std::size_t n>
  struct right_bits<U, n, true, true> {
    static constexpr U value = bit_number_v<U, n + 1> - static_cast<U>(1u);
  };

  template <class U, std::size_t n>
  struct right_bits<U, n, true, false> {
    static constexpr U value = ~0u;
  };

  template <class U, std::size_t n>
  constexpr U right_bits_v = right_bits<U, n>::value;

  // struct left_bits<U, n>.
  template <class U,
            std::size_t n,
            bool = std::is_unsigned_v<U>,
            bool = n <= bit_size_v<U>>
  struct left_bits;

  template <class U, std::size_t n>
  struct left_bits<U, n, true, true> {
  private:
    static constexpr U n_zeros = bit_size_v<U> - n;

  public:
    static constexpr U value = ~right_bits_v<U, n_zeros>;
  };

  template <class U, std::size_t n>
  struct left_bits<U, n, true, false> {
    static constexpr U value = U(-1);
  };

  // U circ_lshift<U>().
  template <class U>
  inline constexpr std::enable_if_t<std::is_unsigned_v<U>, U>
  circ_lshift(U X, U n)
  {
    constexpr std::size_t nbits = bit_size_v<U>;
    constexpr std::size_t mask = nbits - 1ul;
    n %= nbits;
    return (X << n) | (X >> (nbits - n) & mask);
  }
}
#endif /* cetlib_bit_manipulation_h */

// Local Variables:
// mode: c++
// End:
