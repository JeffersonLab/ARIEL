#include "cetlib/bit_manipulation.h"
#include <cstdlib>

using cet::bit_number;
using cet::bit_size;
using cet::circ_lshift;
using cet::left_bits;
using cet::right_bits;

void
ensure(int which, bool claim)
{
  if (not claim)
    std::exit(which);
}

int
main()
{
  typedef unsigned int uint;
  typedef unsigned long ulong;

  ensure(1, bit_size<uint>::value == sizeof(uint) * 8);
  ensure(2, bit_size<ulong>::value == sizeof(ulong) * 8);

  ensure(3, bit_number<uint, 0>::value == 1u);
  ensure(4, bit_number<uint, 1>::value == 2u);
  ensure(5, bit_number<uint, 3>::value == 8u);

  ensure(6, right_bits<uint, 0>::value == 1u);
  ensure(7, right_bits<uint, 1>::value == 3u);
  ensure(8, right_bits<uint, 3>::value == 15u);

  ensure(11,
         (left_bits<uint, 0>::value ^ uint(-1)) == right_bits<uint, 32>::value);
  ensure(12,
         (left_bits<uint, 1>::value ^ uint(-1)) == right_bits<uint, 31>::value);
  ensure(13,
         (left_bits<uint, 3>::value ^ uint(-1)) == right_bits<uint, 29>::value);

  for (uint k = 0, pow2 = 1; k != bit_size<uint>::value; ++k, pow2 *= 2)
    ensure(21 + k, circ_lshift(1u, k) == pow2);

  return 0;

} // main()
