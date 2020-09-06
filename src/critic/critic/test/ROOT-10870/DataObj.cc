#include "DataObj.h"

using namespace r10870;

DataObj::DataObj() noexcept: DataObj(0, 0, 0.0) { }

DataObj::DataObj(int i1, int i2, double f1) noexcept
  : i1_(i1), i2_(i2), f1_(f1)
{
}
