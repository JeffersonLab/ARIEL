#ifndef canvas_root_io_Streamers_RefCoreStreamer_h
#define canvas_root_io_Streamers_RefCoreStreamer_h
// vim: set sw=2 expandtab :

#include "cetlib/exempt_ptr.h"

#include "TClassStreamer.h"

class TBuffer;

namespace art {

  class EDProductGetterFinder;

  class RefCoreStreamer : public TClassStreamer {

  public:
    explicit RefCoreStreamer(
      cet::exempt_ptr<EDProductGetterFinder const> principal =
        cet::exempt_ptr<EDProductGetterFinder const>());

    void setPrincipal(cet::exempt_ptr<EDProductGetterFinder const>);

    virtual TClassStreamer* Generate() const override;

    void operator()(TBuffer&, void*) override;

  private:
    cet::exempt_ptr<EDProductGetterFinder const> principal_;
  };

  void configureRefCoreStreamer(
    cet::exempt_ptr<EDProductGetterFinder const> principal =
      cet::exempt_ptr<EDProductGetterFinder const>());

} // namespace art

#endif /* canvas_root_io_Streamers_RefCoreStreamer_h */

// Local Variables:
// mode: c++
// End:
