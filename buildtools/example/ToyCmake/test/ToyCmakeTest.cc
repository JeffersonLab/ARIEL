#include <iostream>
#include <assert.h>
#include "ToyCmake/Math/DoMath.h"

int main()
{
  ToyCmake::DoMath mth;

  double x,y;
  x = 4.;
  y = x;
  assert( mth.calculate(x) == 16. );

  y = 5.;
  mth.setval(y);
  assert( mth.calculate(x) == 80. );

  return 0;
}

