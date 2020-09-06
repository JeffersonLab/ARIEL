#ifndef canvas_Persistency_Common_HLTPathStatus_h
#define canvas_Persistency_Common_HLTPathStatus_h
// vim: set sw=2 expandtab :

//
//  The status of a single HLT trigger (single trigger path consisting
//  of modules on the path).  Initially, the status is Ready (meaning
//  that this trigger path has not run yet for this event).  If all
//  modules on the path pass (accept) the event, then the state is
//  Pass. If any module on the path fails (rejects) the event, then
//  the state of the whole trigger path is Fail. If any module on the
//  path throws an unhandled error, then the trigger state is
//  Exception.  For the latter two cases, the Fw skips further
//  processing of modules along this path, ie, path processing is
//  aborted.
//
//  The index of the module on the path, 0 to n-1 for a path with n
//  modules issuing the decision for the path is recorded.  For
//  accepted events, this is simply the index of the last module on
//  the path, ie, n-1.
//
//  Note that n is limited, due to packing, to at most 2^(16-2)=16384.
//

#include "canvas/Persistency/Common/HLTenums.h"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace art {
  class HLTPathStatus {
  public: // Special Member Functions
    ~HLTPathStatus();
    HLTPathStatus();
    explicit HLTPathStatus(hlt::HLTState const state);
    explicit HLTPathStatus(hlt::HLTState const state, std::size_t const index);
    HLTPathStatus(HLTPathStatus const&);
    HLTPathStatus(HLTPathStatus&&);
    HLTPathStatus& operator=(HLTPathStatus const&);
    HLTPathStatus& operator=(HLTPathStatus&&);

  public:
    // Get state of path.
    hlt::HLTState state() const;
    // Get index of module on path providing status.
    // This will either be the index of the last module on the
    // path, or the index of the module that rejected or threw.
    unsigned int index() const;
    // Reset this path.
    void reset();
    // Was this path run?
    bool wasrun() const;
    // Has this path accepted the event? If the path was not run, the
    // answer is true.
    bool accept() const;
    // has this path encountered an error (exception)?
    bool error() const;

  private:
    // packed status of trigger path
    // bits 15:2 (0-4095): index of module on path making path decision
    // bits  1:0 (0-3): HLT state
    // Note: We cannot make this atomic because it is persistent,
    // however we can force the alignment.
    alignas(std::uint16_t) std::uint16_t status_;
  };
} // namespace art

#endif /* canvas_Persistency_Common_HLTPathStatus_h */

// Local Variables:
// mode: c++
// End:
