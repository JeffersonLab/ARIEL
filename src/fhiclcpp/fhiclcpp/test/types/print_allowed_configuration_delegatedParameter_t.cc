#include "fhiclcpp/types/DelegatedParameter.h"
#include "fhiclcpp/types/OptionalDelegatedParameter.h"
#include "fhiclcpp/types/Table.h"

#include <iostream>

using namespace fhicl;

namespace {
  struct S {
    DelegatedParameter dp{Name("my_nested_delegate")};
    OptionalDelegatedParameter odp{Name("my_optional_nested_delegate")};
  };

  struct Config {
    DelegatedParameter dp{Name("my_delegate")};
    Table<S> s{Name("s")};
  };
}

int
main()
{
  Table<Config> pset{Name("pset")};
  pset.print_allowed_configuration(std::cout);
}
