#include "critic/test/CriticTestObjects/LiteAssnTestData.h"

std::ostream&
critictest::operator<<(std::ostream& os, LiteAssnTestData const& item)
{
  os << "label: \"" << item.label << "\", d1: " << item.d1
     << ", d2: " << item.d2;
  return os;
}
