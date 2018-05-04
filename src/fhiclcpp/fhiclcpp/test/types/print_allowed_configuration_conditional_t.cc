#include "fhiclcpp/test/types/CondConfig.h"
#include "fhiclcpp/types/Table.h"
#include <iostream>

using namespace fhicl;

int
main()
{
  Table<CondConfig> table{Name("pset")};
  table.print_allowed_configuration(std::cout);
}
