#include "fpc_utils.h"
#include <cmath>

double
arttest::divit(double const x, double const y)
{
  double const result = x / y;
  return result;
}

double
arttest::multit(double const x, double const y)
{
  double const result = x * y;
  return result;
}

double
arttest::logit(double const x)
{
  double const result = std::log(x);
  return result;
}
