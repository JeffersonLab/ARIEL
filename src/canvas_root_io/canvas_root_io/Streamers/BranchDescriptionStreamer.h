#ifndef canvas_root_io_Streamers_BranchDescriptionStreamer_h
#define canvas_root_io_Streamers_BranchDescriptionStreamer_h

#include "TBuffer.h"
#include "TClassStreamer.h"

namespace art {
  class BranchDescription;
  namespace detail {
    class BranchDescriptionStreamer : public TClassStreamer {
    public:
      void operator()(TBuffer&, void* objp) override;
      static void fluffRootTransients(BranchDescription const&);
    };

    void setBranchDescriptionStreamer();
  }
}

#endif /* canvas_root_io_Streamers_BranchDescriptionStreamer_h */

// Local Variables:
// mode: c++
// End:
