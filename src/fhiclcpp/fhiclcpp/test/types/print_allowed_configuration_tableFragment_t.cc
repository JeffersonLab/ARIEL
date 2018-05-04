#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/TableFragment.h"

#include <iostream>
#include <string>

namespace {

  using namespace fhicl;

  //============================================================================
  // Job configuration
  //

  struct S {
    Atom<std::string> test{Name("test"), Comment("this works")};
    Sequence<int> sequence{Name("sequence")};
    Sequence<double, 5> array{Name("array")};
  };

  struct Config {
    TableFragment<S> test;
  };
}

int
main()
{
  Table<Config> pset{Name("pset")};
  pset.print_allowed_configuration(std::cout);
}
