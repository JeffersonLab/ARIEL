#include "canvas/Persistency/Common/RefCore.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/EDProductGetter.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/Exception.h"

#include <algorithm>
#include <cassert>

namespace art {

  RefCore::RefCoreTransients::RefCoreTransients(
    void const* prodPtr,
    EDProductGetter const* prodGetter) noexcept
    : itemPtr_{prodPtr}, prodGetter_{prodGetter}
  {}

  RefCore::RefCore(ProductID const& id,
                   void const* prodPtr,
                   EDProductGetter const* prodGetter)
    : id_{id}, transients_{prodPtr, prodGetter}
  {}

  bool
  RefCore::isAvailable() const
  {
    if (productPtr()) {
      return true;
    }
    return id_.isValid() && productGetter() != nullptr &&
           productGetter()->getIt() != nullptr;
  }

  void const*
  RefCore::productPtr() const noexcept
  {
    return transients_.itemPtr_;
  }

  // Used by isAvailable()
  EDProductGetter const*
  RefCore::productGetter() const noexcept
  {
    return transients_.prodGetter_;
  }

  // Used by RefCoreStreamer on read to set itemPtr_
  // to the nullptr.
  void
  RefCore::setProductPtr(void const* prodPtr) const noexcept
  {
    transients_.itemPtr_ = prodPtr;
  }

  // Used by RefCoreStreamer on read.
  // Note: prodGetter_ is actually a Group for art,
  //       and a BranchData or AssnsBranchData for gallery.
  void
  RefCore::setProductGetter(EDProductGetter const* prodGetter) noexcept
  {
    transients_.prodGetter_ = prodGetter;
  }

  void
  RefCore::swap(RefCore& other)
  {
    std::swap(id_, other.id_);
    std::swap(transients_, other.transients_);
  }

  void
  swap(RefCore& lhs, RefCore& rhs)
  {
    lhs.swap(rhs);
  }

} // namespace art
