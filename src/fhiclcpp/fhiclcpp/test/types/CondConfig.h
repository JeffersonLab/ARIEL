#ifndef fhiclcpp_test_types_CondConfig_h
#define fhiclcpp_test_types_CondConfig_h

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/TupleAs.h"

#include <array>
#include <ostream>
#include <string>

namespace fhicl {

  class Vec {
    std::array<int, 3> hls_;

  public:
    Vec(int i, int j, int k) : hls_{{i, j, k}} {}
    auto const&
    operator()() const
    {
      return hls_;
    }
  };

  std::ostream&
  operator<<(std::ostream& os, Vec const& v)
  {
    return os << "Half-lengths are: (" << v()[0] << ", " << v()[1] << ", "
              << v()[2] << ")";
  }

  struct CondConfig {

    Atom<bool> flag{Name("flag")};

    bool
    flag_is_true() const
    {
      return flag();
    }

    OptionalAtom<int> num1{Name("num1"),
                           Comment("Used if \"flag: true\"."),
                           fhicl::use_if(this, &CondConfig::flag_is_true)};

    Atom<int> num2{Name("num2"),
                   Comment("Used if \"flag: false\"."),
                   use_unless(this, &CondConfig::flag_is_true)};

    Atom<std::string> shape{Name("shape"), "box"};

    struct BoxParameters {
      TupleAs<Vec(int, int, int)> halfLengths{Name("halfLengths"),
                                              Vec{1, 2, 3}};
      OptionalAtom<std::string> boxName{Name("boxName")};
      Atom<std::string> material{Name("material")};
    };
    Table<BoxParameters> boxParams{
      Name("parameters"),
      Comment("Used if \"shape: box\"."),
      MaybeUseFunction([this]() { return shape() == "box"; })};

    struct SphereParameters {
      Atom<int> radius{Name("radius")};
    };
    Table<SphereParameters> sphereParams{
      Name("parameters"),
      Comment("Used if \"shape: sphere\"."),
      MaybeUseFunction([this]() { return shape() == "sphere"; })};
  };
}

#endif /* fhiclcpp_test_types_CondConfig_h */

// Local variables:
// mode: c++
// End:
