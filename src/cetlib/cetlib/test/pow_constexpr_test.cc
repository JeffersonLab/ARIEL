#include "cetlib/pow.h"

int
main()
{
  static_assert(cet::pow<2>(3) == 9, "pow fail 1");
  static_assert(cet::pow<3>(2) == 8, "pow fail 2");
  static_assert(cet::pow<2>(0.5) == 0.25, "pow fail 3");
  static_assert(cet::pow<1>(0.0f) == 0.0, "pow fail 4");

  static_assert(cet::diff_of_squares(13, 12) == 25, "diff_of_squares fail 1");
  static_assert(cet::diff_of_squares(13u, 12u) == 25u,
                "diff_of_squares fail 2");
  static_assert(cet::diff_of_squares(13L, 12L) == 25L,
                "diff_of_squares fail 3");
  static_assert(cet::diff_of_squares(13., 12.) == 25.,
                "diff_of_squares fail 4");

  static_assert(cet::sum_of_squares(13, 12) == 313, "sum_of_squares fail 1");
  static_assert(cet::sum_of_squares(13u, 12u) == 313u, "sum_of_squares fail 2");
  static_assert(cet::sum_of_squares(13L, 12L) == 313L, "sum_of_squares fail 3");
  static_assert(cet::sum_of_squares(13., 12.) == 313., "sum_of_squares fail 4");
}
