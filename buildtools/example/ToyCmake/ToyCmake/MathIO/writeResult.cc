#include <iostream>
#include "ToyCmake/MathIO/writeResult.h"

namespace ToyCmake {

std::ostream&  writeResult(double x, double y, std::ostream & os )
{
    os << "math on " << x << " results in " << y << std::endl;
    return os;
}

}

