#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/OptionalSequence.h"
#include "fhiclcpp/types/OptionalTable.h"
#include "fhiclcpp/types/OptionalTuple.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/Tuple.h"

#include <iostream>

namespace {

  using namespace fhicl;

  //============================================================================
  // Job configuration
  //

  struct Physics {
    Tuple<std::string, double> energyCutoff{Name("energyCutoff")};
    OptionalAtom<std::string> moniker{Name("moniker")};
  };

  struct Composer {
    Atom<std::string> composer{Name("composer")};
    OptionalTuple<int, std::string> aSymphonyMoniker{
      Name("aSymphonyMoniker"),
      Comment("Symphony number/nickname pair")};
  };

  struct Configuration {
    OptionalAtom<int> atom{Name("atom")};
    OptionalAtom<std::string> name{Name("name")};
    OptionalTable<Physics> physics{Name("physics")};
    OptionalSequence<int> list1{Name("list1")};
    OptionalSequence<int, 4> list2{Name("list2")};
    OptionalSequence<Sequence<int, 2>> list3{Name("list3")};
    OptionalSequence<Table<Composer>, 4> list4{Name("list4")};
  };
}

int
main()
{
  Table<Configuration> pset{Name("pset")};
  pset.print_allowed_configuration(std::cout);
}
