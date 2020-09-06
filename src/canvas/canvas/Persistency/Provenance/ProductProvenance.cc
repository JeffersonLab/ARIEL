// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ProductProvenance.h"
#include "canvas/Persistency/Provenance/ParentageRegistry.h"
#include "canvas/Utilities/Exception.h"

#include <cassert>
#include <ostream>
#include <utility>

using namespace std;

namespace art {

  ProductProvenance::ProductProvenance() = default;
  ProductProvenance::~ProductProvenance() = default;

  ProductProvenance::ProductProvenance(ProductID const& bid,
                                       ProductStatus const status)
    : productID_{bid}, productStatus_{status}, parentageID_{}, transients_{}
  {}

  ProductProvenance::ProductProvenance(ProductID const& bid,
                                       ProductStatus const status,
                                       vector<ProductID> const& parents)
    : productID_{bid}, productStatus_{status}, parentageID_{}, transients_{}
  {
    transients_.get().parentagePtr_ = make_shared<Parentage>(parents);
    parentageID_ = transients_.get().parentagePtr_->id();
    ParentageRegistry::emplace(parentageID_, *transients_.get().parentagePtr_);
  }

  ProductProvenance::ProductProvenance(ProductProvenance const& rhs) = default;
  ProductProvenance::ProductProvenance(ProductProvenance&& rhs) = default;
  ProductProvenance& ProductProvenance::operator=(
    ProductProvenance const& rhs) = default;
  ProductProvenance& ProductProvenance::operator=(ProductProvenance&& rhs) =
    default;

  ProductID
  ProductProvenance::productID() const noexcept
  {
    return productID_;
  }

  ProductStatus
  ProductProvenance::productStatus() const noexcept
  {
    return productStatus_;
  }

  ParentageID const&
  ProductProvenance::parentageID() const noexcept
  {
    return parentageID_;
  }

  // Note: This is true for Run, SubRun, and Results products.
  bool
  ProductProvenance::noParentage() const noexcept
  {
    return transients_.get().noParentage_;
  }

  Parentage const&
  ProductProvenance::parentage() const
  {
    if (!transients_.get().parentagePtr_) {
      transients_.get().parentagePtr_ = make_shared<Parentage>();
      ParentageRegistry::get(parentageID_, *transients_.get().parentagePtr_);
    }
    return *transients_.get().parentagePtr_;
  }

  void
  ProductProvenance::write(ostream& os) const
  {
    os << "product ID = " << productID() << '\n';
    os << "product status = " << static_cast<int>(productStatus_) << '\n';
    if (!noParentage()) {
      os << "entry description ID = " << parentageID() << '\n';
    }
  }

  ostream&
  operator<<(ostream& os, ProductProvenance const& p)
  {
    p.write(os);
    return os;
  }

  bool
  operator==(ProductProvenance const& a, ProductProvenance const& b) noexcept
  {
    if (a.noParentage() != b.noParentage()) {
      return false;
    }
    if (a.noParentage()) {
      return (a.productID() == b.productID()) &&
             (a.productStatus() == b.productStatus());
    }
    return (a.productID() == b.productID()) &&
           (a.productStatus() == b.productStatus()) &&
           (a.parentageID() == b.parentageID());
  }

  bool
  operator!=(ProductProvenance const& a, ProductProvenance const& b) noexcept
  {
    return !(a == b);
  }

  // FIXME: This is not compatible with operator==.  That is if !(a<b) && !(a>b)
  // does not imply (a==b)!!!
  bool
  operator<(ProductProvenance const& a, ProductProvenance const& b) noexcept
  {
    return a.productID() < b.productID();
  }

} // namespace art
