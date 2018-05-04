// ======================================================================
//
// test printing for tupleAs
//
// ======================================================================

#include "fhiclcpp/types/OptionalTupleAs.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/TupleAs.h"

#include <iostream>
#include <string>
#include <vector>

using namespace fhicl;
using namespace std;

namespace {

  struct ThreeNumbers {
    int i_, j_, k_;
    ThreeNumbers(int i, int j, int k) : i_{i}, j_{j}, k_{k} {}
  };

  struct Person {
    string name_;
    unsigned age_{};
    Person() = default;
    Person(string name, unsigned age) : name_{name}, age_{age} {}
  };

  struct ToVector {

    ToVector(int j, std::vector<int> const& v)
    {
      for (auto number : v)
        value_.push_back(j * number);
    }

    std::vector<int> value_;
  };

  std::ostream&
  operator<<(std::ostream& os, Person const& p)
  {
    return os << "Name: " << p.name_ << "  Age: " << p.age_;
  }

  struct Config {
    TupleAs<ThreeNumbers(int, int, int)> tn1{Name("numbers1"),
                                             Comment("Vector for Geant")};
    TupleAs<ThreeNumbers(int, int, int)> tn2{Name("numbers2"),
                                             ThreeNumbers{1, 3, 5}};
    OptionalTupleAs<Person(string, unsigned)> person1{Name("person1")};
    TupleAs<Person(string, unsigned)> person2{Name("person2"),
                                              Person{"Jon", 97}};
    TupleAs<ToVector(int, Sequence<int>)> toVector{Name("toVector")};
    Sequence<TupleAs<Person(string, unsigned)>> people{Name("people")};
  };
}

int
main()
{
  fhicl::Table<Config> const config{Name("pset")};
  config.print_allowed_configuration(cout);
}
