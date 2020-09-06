#include "cetlib/pow.h"

int
main()
{
  static_assert(cet::pow<2>(3) == 9);
  static_assert(cet::pow<3>(2) == 8);
  static_assert(cet::pow<2>(0.5) == 0.25);
  static_assert(cet::pow<1>(0.0f) == 0.0);

  static_assert(cet::diff_of_squares(13, 12) == 25);
  static_assert(cet::diff_of_squares(13u, 12u) == 25u);
  static_assert(cet::diff_of_squares(13L, 12L) == 25L);
  static_assert(cet::diff_of_squares(13., 12.) == 25.);

  static_assert(cet::sum_of_squares(13, 12) == 313);
  static_assert(cet::sum_of_squares(13u, 12u) == 313u);
  static_assert(cet::sum_of_squares(13L, 12L) == 313L);
  static_assert(cet::sum_of_squares(13., 12.) == 313.);
}
