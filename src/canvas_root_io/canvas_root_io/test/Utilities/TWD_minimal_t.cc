#include <vector>

#include "TClass.h"

int
main()
{
  auto const result [[gnu::unused]] = TClass::GetClass("std::vector<int>");
}
