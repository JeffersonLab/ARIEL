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
    Atom<std::string> test{Name("test"), Comment("this works")};
    Sequence<int> sequence{Name("sequence")};
    Sequence<double, 5> array{Name("array")};
  };

  struct Test {
    Atom<int> value{Name("c")};
  };

  struct ForRootInput {
    Atom<std::string> primary{Name("a")};
    Sequence<std::string> secondaries{Name("b")};
    Sequence<Table<Test>> tablesequence2{Name("tableSequence2")};
  };

  struct Configuration {
    Atom<bool> flag{Name("flag")};

    Tuple<double, std::string> pair{Name("pair")};
    Tuple<int, std::string, double> tup{Name("tuple")};

    Sequence<Tuple<int, double>> vt{Name("vecOfTuples")};
    Sequence<Tuple<int, double>, 2> at{Name("arrayOfTuples")};

    Sequence<Sequence<int>> vv{Name("vecOfVecs")};
    Sequence<Sequence<int, 2>> va{Name("vecOfArrays")};
    Sequence<Sequence<int>, 2> av{Name("arrayOfVecs")};
    Sequence<Sequence<int, 2>, 2> aa{Name("arrayOfArrays")};

    Tuple<Sequence<double>, bool, int> tv{Name("tupleWithVec")};
    Tuple<Sequence<double, 2>, bool, int> ta{Name("tupleWithArray")};
    Tuple<Tuple<int, double>, bool, int> tt{Name("tupleWithTuple")};

    Sequence<Table<ForRootInput>> vtable{Name("vecOfTables")};
    Sequence<Table<ForRootInput>, 2> atable{Name("arrayOfTables")};
    Tuple<Table<ForRootInput>, bool, int> tupleWithTable{
      Name("tupleWithTable")};
    Tuple<Sequence<Table<ForRootInput>>, double> tvtable{
      Name("tupleWithVecTable")};
    Tuple<Sequence<Table<ForRootInput>, 2>, double> tstable{
      Name("tupleWithArrTable")};
    Sequence<Tuple<Table<ForRootInput>, double>> vttable{
      Name("vecWithTupleTable")};
    Sequence<Tuple<Table<ForRootInput>, double>, 2> attable{
      Name("arrWithTupleTable")};
    Table<PSet2> pset2{Name("pset2")};
  };
}

int
main()
{
  Table<Configuration> pset{Name("pset")};
  pset.print_allowed_configuration(std::cout);
}
