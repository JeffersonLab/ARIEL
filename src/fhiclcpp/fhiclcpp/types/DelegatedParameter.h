#ifndef fhiclcpp_types_DelegatedParameter_h
#define fhiclcpp_types_DelegatedParameter_h

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/detail/DelegateBase.h"
#include "fhiclcpp/types/detail/ParameterArgumentTypes.h"
#include "fhiclcpp/types/detail/TableMemberRegistry.h"
#include "fhiclcpp/types/detail/strip_containing_names.h"

#include <iostream>

namespace fhicl {

  //========================================================
  class DelegatedParameter final : public detail::DelegateBase,
                                   private detail::RegisterIfTableMember {
  public:
    explicit DelegatedParameter(Name&& name);
    explicit DelegatedParameter(Name&& name, Comment&& comment);
    explicit DelegatedParameter(Name&& name,
                                Comment&& comment,
                                std::function<bool()> maybeUse);

    // A DelegatedParameter object must be present.  Therefore, it is
    // safe and correct to call 'pset.get' with no default.
    template <typename T>
    auto
    get() const
    {
      std::string const& trimmed_key =
        detail::strip_first_containing_name(key());
      return pset_.get<T>(trimmed_key);
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

#endif /* fhiclcpp_types_DelegatedParameter_h */

// Local variables:
// mode: c++
// End:
