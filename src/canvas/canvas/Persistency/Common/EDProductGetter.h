#ifndef canvas_Persistency_Common_EDProductGetter_h
#define canvas_Persistency_Common_EDProductGetter_h

#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/fwd.h"

namespace art {
  class EDProduct;
  class EDProductGetter;
}

class art::EDProductGetter {
public:
  EDProductGetter() = default;
  EDProductGetter(EDProductGetter const&) = delete;
  EDProductGetter& operator=(EDProductGetter const&) = delete;
  virtual ~EDProductGetter() = default;

  // Can you even try to resolve the product?
  virtual bool isReady() const = 0;
  virtual EDProduct const* getIt() const = 0;
  virtual EDProduct const* anyProduct() const = 0;
  virtual EDProduct const* uniqueProduct() const = 0;
  virtual EDProduct const* uniqueProduct(TypeID const&) const = 0;
  virtual bool resolveProduct(TypeID const&) const = 0;
  virtual bool resolveProductIfAvailable(TypeID const&) const = 0;
}; // EDProductGetter

#endif /* canvas_Persistency_Common_EDProductGetter_h */

// Local Variables:
// mode: c++
// End:
