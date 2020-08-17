#ifndef canvas_Persistency_Common_EDProductGetter_h
#define canvas_Persistency_Common_EDProductGetter_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/fwd.h"

namespace art {

  class EDProduct;

  class EDProductGetter {

  public:
    virtual ~EDProductGetter();

    EDProductGetter();

    EDProductGetter(EDProductGetter const&) = delete;

    EDProductGetter(EDProductGetter&&) = delete;

    EDProductGetter& operator=(EDProductGetter const&) = delete;

    EDProductGetter& operator=(EDProductGetter&&) = delete;

  public:
    EDProduct const* getIt() const;

  private:
    virtual EDProduct const* getIt_() const;
  };

} // namespace art

#endif /* canvas_Persistency_Common_EDProductGetter_h */

// Local Variables:
// mode: c++
// End:
