#ifndef canvas_root_io_Streamers_RefCoreStreamer_h
#define canvas_root_io_Streamers_RefCoreStreamer_h
// vim: set sw=2 expandtab :

#include "TClassStreamer.h"

#include <atomic>

class TBuffer;

namespace art {

  class PrincipalBase;

  class RefCoreStreamer : public TClassStreamer {
  public:
    explicit RefCoreStreamer(PrincipalBase const* principal = nullptr);
    void setPrincipal(PrincipalBase const*);
    virtual TClassStreamer* Generate() const override;
    void operator()(TBuffer&, void*) override;

  private:
    std::atomic<PrincipalBase const*> principal_;
  };

  void configureRefCoreStreamer(PrincipalBase const* principal = nullptr);

} // namespace art

#endif /* canvas_root_io_Streamers_RefCoreStreamer_h */

// Local Variables:
// mode: c++
// End:
