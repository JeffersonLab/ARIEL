#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/Tuple.h"

#include <iostream>

namespace {

  using namespace fhicl;
  //============================================================================
  // Job configuration
  //

  struct PSet2 {
    Atom<std::string> test{Name("test"), "a"};
    Sequence<int> sequence{Name("sequence"),
                           Comment("Just to see if this works"),
                           {1, 3, 67, 5}};
    Sequence<double, 5> array{Name("array"), {1.4, 2.3, 4., 8., 123098.}};
  };

  struct Test {
    Atom<int> value{Name("c"), 4};
  };

  struct ForRootInput {
    Atom<std::string> primary{Name("a"), "cool"};
    Sequence<std::string> secondaries{Name("b")};
  };

  struct Configuration {

    Atom<bool> flag{Name("flag"), true};
    Atom<int> value{Name("value"), 7};
    Atom<std::string> str{Name("string"), "hi mom"};

    Tuple<double, std::string> pair{
      Name("pair"),
      {2.2345678, "Finally, I know what I'm doing."}};
    Tuple<int, std::string, double> tup{Name("tuple"), {5, "some string", 16.}};

    Sequence<Tuple<int, double>> vt{Name("vecOfTuples"), {{3, 27.}}};
    Sequence<Tuple<int, double>, 2> at{Name("arrayOfTuples"),
                                       {{4, 15.}, {2, 17.}}};

    Sequence<Sequence<int>> vv{Name("vecOfVecs"), {{1, 3, 5}}};
    Sequence<Sequence<int, 2>> va{Name("vecOfArrays"), {{1, 2}}};
    Sequence<Sequence<int>, 2> av{Name("arrayOfVecs"), {{1, 2, 3}, {1}}};
    Sequence<Sequence<int, 2>, 2> aa{Name("arrayOfArrays"), {{0, 2}, {2, 3}}};

    Tuple<Sequence<double>, bool, int> tv{Name("tupleWithVec"),
                                          {{4.2, 7.3}, false, 5}};
    Tuple<Sequence<double, 2>, bool, int> ta{Name("tupleWithArray"),
                                             {{3.6, 5.3}, false, 5}};
    Tuple<Tuple<int, double>, bool, int> tt{Name("tupleWithTuple"),
                                            {{4, 5.9}, true, 8}};

    Table<PSet2> pset2{Name("pset2"),
                       Comment("This should work now\ndouble line even.")};
  };
}

int
main()
{
  Table<Configuration> pset{Name("pset")};
  pset.print_allowed_configuration(std::cout);
}
