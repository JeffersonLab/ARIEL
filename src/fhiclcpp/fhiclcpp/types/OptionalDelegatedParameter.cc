#include "fhiclcpp/types/OptionalDelegatedParameter.h"
#include "cetlib/container_algorithms.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"

namespace fhicl {

  OptionalDelegatedParameter::OptionalDelegatedParameter(Name&& name)
    : OptionalDelegatedParameter{std::move(name), Comment("")}
  {}

  OptionalDelegatedParameter::OptionalDelegatedParameter(Name&& name,
                                                         Comment&& comment)
    : DelegateBase{std::move(name),
                   std::move(comment),
                   par_style::OPTIONAL,
                   detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    NameStackRegistry::end_of_ctor();
  }

  OptionalDelegatedParameter::OptionalDelegatedParameter(
    Name&& name,
    Comment&& comment,
    std::function<bool()> maybeUse)
    : DelegateBase{std::move(name),
                   std::move(comment),
                   par_style::OPTIONAL_CONDITIONAL,
                   maybeUse}
    , RegisterIfTableMember{this}
  {
    NameStackRegistry::end_of_ctor();
  }
}
