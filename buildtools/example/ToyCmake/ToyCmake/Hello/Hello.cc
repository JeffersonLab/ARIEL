#include <iostream>
#include <assert.h>
#include "ToyCmake/Math/DoMath.h"

int main()
{
  ToyCmake::DoMath mth;

  double x,y;
  x = 4.;
  assert( mth.calculate(x) == 16. );

  y = 5.;
  mth.setval(y);
  assert( mth.calculate(x) == 80. );
  
  std::cout << "Hello, I've just done some math calculations" << std::endl;
  std::cout << "input " << x <<","<< y << " produces " << mth.calculate(x) << std::endl;

  return 0;
}

