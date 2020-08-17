#include "canvas/Persistency/Common/TriggerResults.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTGlobalStatus.h"
#include "canvas/Persistency/Common/traits.h"
#include "fhiclcpp/ParameterSetID.h"

using namespace std;

namespace art {

  TriggerResults::~TriggerResults() noexcept = default;
  TriggerResults::TriggerResults() = default;

  TriggerResults::TriggerResults(HLTGlobalStatus const& hlt,
                                 fhicl::ParameterSetID const& psetID)
    : HLTGlobalStatus{hlt}, psetid_{psetID}
  {}

  fhicl::ParameterSetID const&
  TriggerResults::parameterSetID() const
  {
    return psetid_;
  }

} // namespace art
