#ifndef fhiclcpp_types_detail_AtomBase_h
#define fhiclcpp_types_detail_AtomBase_h

#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/detail/ParameterBase.h"

namespace fhicl {

  class ParameterSet;

  namespace detail {

    //========================================================
    class AtomBase : public ParameterBase {
    public:
      AtomBase(Name const& name,
               Comment const& comment,
               par_style const vt,
               std::function<bool()> maybeUse)
        : ParameterBase{name, comment, vt, par_type::ATOM, maybeUse}
      {}

      std::string
      stringified_value() const
      {
        return get_stringified_value();
      }

    private:
      virtual std::string get_stringified_value() const = 0;
    };
  }
}

#endif /* fhiclcpp_types_detail_AtomBase_h */

// Local variables:
// mode: c++
// End:
