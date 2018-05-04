#ifndef canvas_root_io_Streamers_CacheStreamers_h
#define canvas_root_io_Streamers_CacheStreamers_h
// vim: set sw=2 expandtab :

#include "TClassRef.h"
#include "TClassStreamer.h"
class TBuffer;

namespace art {

  class ConstPtrCacheStreamer : public TClassStreamer {

  public:
    explicit ConstPtrCacheStreamer() : cl_("art::ConstPtrCache") {}

    virtual TClassStreamer* Generate() const override;

    void operator()(TBuffer& R_b, void* objp) override;

  private:
    TClassRef cl_;
  };

  class BoolCacheStreamer : public TClassStreamer {

  public:
    explicit BoolCacheStreamer() : cl_("art::BoolCache") {}

    virtual TClassStreamer* Generate() const override;

    void operator()(TBuffer& R_b, void* objp) override;

  private:
    TClassRef cl_;
  };

  void setCacheStreamers();

} // namespace art

#endif /* canvas_root_io_Streamers_CacheStreamers_h */

// Local Variables:
// mode: c++
// End:
