#include "canvas/Persistency/Provenance/ProductProvenance.h"
#include "canvas/Persistency/Provenance/ParentageRegistry.h"
#include "canvas/Utilities/Exception.h"

#include <cassert>
#include <ostream>

namespace art {

  ProductProvenance::ProductProvenance(ProductID const pid) : productID_{pid} {}

  ProductProvenance::ProductProvenance(ProductID const pid,
                                       ProductStatus const status)
    : productID_{pid}, productStatus_{status}
  {}

  ProductProvenance::ProductProvenance(ProductID const pid,
                                       ProductStatus const status,
                                       ParentageID const& edid)
    : productID_{pid}, productStatus_{status}, parentageID_{edid}
  {}

  ProductProvenance::ProductProvenance(ProductID const pid,
                                       ProductStatus const status,
                                       std::shared_ptr<Parentage> pPtr)
    : productID_{pid}, productStatus_{status}, parentageID_{pPtr->id()}
  {
    parentagePtr() = pPtr;
    ParentageRegistry::emplace(parentageID_, *pPtr);
  }

  ProductProvenance::ProductProvenance(ProductID const pid,
                                       ProductStatus const status,
                                       std::vector<ProductID> const& parents)
    : productID_{pid}, productStatus_{status}
  {
    parentagePtr() = std::make_shared<Parentage>();
    parentagePtr()->parents() = parents;
    parentageID_ = parentagePtr()->id();
    ParentageRegistry::emplace(parentageID_, *parentagePtr());
  }

  Parentage const&
  ProductProvenance::parentage() const
  {
    if (!parentagePtr()) {
      parentagePtr() = std::make_shared<Parentage>();
      ParentageRegistry::get(
        parentageID_, *parentagePtr()); // Filled only if successful retrieval
    }
    return *parentagePtr();
  }

  void
  ProductProvenance::setPresent() const noexcept
  {
    setStatus(productstatus::present());
  }

  void
  ProductProvenance::setNotPresent() const noexcept
  {
    if (productstatus::dropped(productStatus()))
      return;
    setStatus(productstatus::neverCreated());
  }

  void
  ProductProvenance::write(std::ostream& os) const
  {
    os << "product ID = " << productID() << '\n';
    os << "product status = " << static_cast<int>(productStatus()) << '\n';
    if (!noParentage()) {
      os << "entry description ID = " << parentageID() << '\n';
    }
  }

  // Only the 'salient attributes' are tested in equality comparison.
  bool
  operator==(ProductProvenance const& a, ProductProvenance const& b) noexcept
  {
    if (a.noParentage() != b.noParentage())
      return false;
    if (a.noParentage()) {
      return a.productID() == b.productID() &&
             a.productStatus() == b.productStatus();
    }
    return a.productID() == b.productID() &&
           a.productStatus() == b.productStatus() &&
           a.parentageID() == b.parentageID();
  }
}
