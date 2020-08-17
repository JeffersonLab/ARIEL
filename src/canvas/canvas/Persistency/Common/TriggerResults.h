#ifndef canvas_Persistency_Common_TriggerResults_h
#define canvas_Persistency_Common_TriggerResults_h
// vim: set sw=2 expandtab :

//
//  The trigger path results are maintained here as a sequence of entries,
//  one per trigger path.  They are assigned in the order they appeared in
//  the process-level pset.  (They are actually stored in the base class
//  HLTGlobalStatus)
//
//  The ParameterSetID can be used to get a ParameterSet from the registry
//  of parameter sets.  This ParameterSet contains a vector<string> named
//  "trigger_paths" that contains the trigger path names in the same order
//  as the trigger path results stored here.
//

#include "canvas/Persistency/Common/HLTGlobalStatus.h"
#include "fhiclcpp/ParameterSetID.h"

namespace art {

  class TriggerResults : public HLTGlobalStatus {
  public:
    ~TriggerResults() noexcept;
    TriggerResults();

    TriggerResults(HLTGlobalStatus const& hlt,
                   fhicl::ParameterSetID const& psetid);

  public:
    fhicl::ParameterSetID const& parameterSetID() const;

  private:
    fhicl::ParameterSetID psetid_{};
  };

  void swap(TriggerResults& lhs, TriggerResults& rhs);

} // namespace art

#endif /* canvas_Persistency_Common_TriggerResults_h */

// Local Variables:
// mode: c++
// End:
