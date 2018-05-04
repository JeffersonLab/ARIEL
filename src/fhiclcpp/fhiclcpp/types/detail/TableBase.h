#ifndef fhiclcpp_types_detail_TableBase_h
#define fhiclcpp_types_detail_TableBase_h

#include "cetlib/exempt_ptr.h"
#include "fhiclcpp/types/detail/ParameterBase.h"

namespace fhicl {

  class ParameterSet;

  namespace detail {

    //========================================================
    class TableBase : public ParameterBase {
    public:
      TableBase(Name const& name,
                Comment const& comment,
                par_style const vt,
                std::function<bool()> maybeUse)
        : ParameterBase{name, comment, vt, par_type::TABLE, maybeUse}
      {}

      std::vector<cet::exempt_ptr<ParameterBase>> const&
      members() const
      {
        return get_members();
      }

    private:
      virtual std::vector<cet::exempt_ptr<ParameterBase>> const& get_members()
        const = 0;
    };
  }
}

#endif /* fhiclcpp_types_detail_TableBase_h */

// Local variables:
// mode: c++
// End:
