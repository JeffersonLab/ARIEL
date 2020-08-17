#ifndef fhiclcpp_types_OptionalDelegatedParameter_h
#define fhiclcpp_types_OptionalDelegatedParameter_h

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/detail/DelegateBase.h"
#include "fhiclcpp/types/detail/ParameterArgumentTypes.h"
#include "fhiclcpp/types/detail/TableMemberRegistry.h"
#include "fhiclcpp/types/detail/strip_containing_names.h"

namespace fhicl {

  //========================================================
  class OptionalDelegatedParameter final
    : public detail::DelegateBase,
      private detail::RegisterIfTableMember {
  public:
    explicit OptionalDelegatedParameter(Name&& name);
    explicit OptionalDelegatedParameter(Name&& name, Comment&& comment);
    explicit OptionalDelegatedParameter(Name&& name,
                                        Comment&& comment,
                                        std::function<bool()> maybeUse);
    bool
    hasValue() const
    {
      return pset_.has_key(detail::strip_first_containing_name(key()));
    }

    template <typename T>
    bool
    get_if_present(T& t) const
    {
      auto const trimmed_key = detail::strip_first_containing_name(key());
      return pset_.get_if_present<T>(trimmed_key, t);
    }

  private:
    void
    do_set_value(fhicl::ParameterSet const& pset,
                 bool const /*trimParents*/) override
    {
      pset_ = pset;
    };

    fhicl::ParameterSet pset_{};
  };
}

#endif /* fhiclcpp_types_OptionalDelegatedParameter_h */

// Local variables:
// mode: c++
// End:
