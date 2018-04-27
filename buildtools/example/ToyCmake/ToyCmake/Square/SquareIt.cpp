// This is serious overkill for the sake of an example

#include "ToyCmake/Math/DoMath.h"
#include "ToyCmake/MathIO/writeResult.h"

int main()
{
   ToyCmake::DoMath dm(2.);
   double x = 5.;
   double y = dm.calculate(x);
   ToyCmake::writeResult(x,y);
   return 0;
}

