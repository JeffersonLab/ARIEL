#ifndef fhiclcpp_test_types_KeyMap_h
#define fhiclcpp_test_types_KeyMap_h

#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/detail/ParameterWalker.h"

#include <string>
#include <vector>

namespace fhicl {

  class KeyMap
    : public detail::ParameterWalker<tt::const_flavor::require_const> {
  public:
    auto const&
    result() const
    {
      return keys_;
    };

    template <typename T>
    static auto
    get(std::string const& n)
    {
      KeyMap km;
      T test{Name(n)};
      km.walk_over(test);
      return km.result();
    }

  private:
    std::vector<std::string> keys_;

    void
    enter_table(detail::TableBase const& tb) override
    {
      append(tb.key());
    }
    void
    enter_sequence(detail::SequenceBase const& sb) override
    {
      append(sb.key());
    }
    void
    atom(detail::AtomBase const& ab) override
    {
      append(ab.key());
    }
    void
    delegated_parameter(detail::DelegateBase const& dp) override
    {
      append(dp.key());
    }

    void
    append(std::string const& k)
    {
      keys_.emplace_back(k);
    }
  };
}

#endif /* fhiclcpp_test_types_KeyMap_h */

// Local variables:
// mode: c++
// End:
