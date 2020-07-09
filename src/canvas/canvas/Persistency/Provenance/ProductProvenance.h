#ifndef canvas_Persistency_Provenance_ProductProvenance_h
#define canvas_Persistency_Provenance_ProductProvenance_h

/*----------------------------------------------------------------------

ProductProvenance: The event dependent portion of the description of a product
and how it came into existence, plus the status.

----------------------------------------------------------------------*/

#include "canvas/Persistency/Provenance/ParentageID.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProductStatus.h"
#include "canvas/Persistency/Provenance/ProvenanceFwd.h"
#include "canvas/Persistency/Provenance/Transient.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace art {
  class ProductProvenance;
  using ProductProvenances = std::vector<ProductProvenance>;

  bool operator<(ProductProvenance const& a, ProductProvenance const& b) noexcept;
  std::ostream& operator<<(std::ostream& os, ProductProvenance const& p);
  bool operator==(ProductProvenance const& a, ProductProvenance const& b) noexcept;
  bool operator!=(ProductProvenance const& a, ProductProvenance const& b) noexcept;
}

class art::ProductProvenance {
public:
  ProductProvenance() = default;
  explicit ProductProvenance(ProductID pid);

  ProductProvenance(ProductID pid, ProductStatus status);

  ProductProvenance(ProductID pid,
                    ProductStatus status,
                    std::shared_ptr<Parentage> parentagePtr);

  ProductProvenance(ProductID pid, ProductStatus status, ParentageID const& id);

  ProductProvenance(ProductID pid,
                    ProductStatus status,
                    std::vector<ProductID> const& parents);

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  void write(std::ostream& os) const;

  ProductID
  productID() const noexcept
  {
    return productID_;
  }
  ProductStatus
  productStatus() const noexcept
  {
    return productStatus_;
  }
  ParentageID const&
  parentageID() const noexcept
  {
    return parentageID_;
  }
  Parentage const& parentage() const;
  void
  setStatus(ProductStatus const status) const noexcept
  {
    productStatus_ = status;
  }
  void setPresent() const noexcept;
  void setNotPresent() const noexcept;

  bool
  noParentage() const noexcept
  {
    return transients_.get().noParentage_;
  }

  struct Transients {
    Transients() = default;
    std::shared_ptr<Parentage> parentagePtr_{nullptr};
    bool noParentage_{false};
  };

private:
  std::shared_ptr<Parentage>&
  parentagePtr() const noexcept
  {
    return transients_.get().parentagePtr_;
  }

  ProductID productID_{};
  mutable ProductStatus productStatus_{productstatus::uninitialized()};
  ParentageID parentageID_{};
  mutable Transient<Transients> transients_{};
};

inline bool
art::operator<(ProductProvenance const& a, ProductProvenance const& b) noexcept
{
  return a.productID() < b.productID();
}

inline std::ostream&
art::operator<<(std::ostream& os, ProductProvenance const& p)
{
  p.write(os);
  return os;
}

inline bool
art::operator!=(ProductProvenance const& a, ProductProvenance const& b) noexcept
{
  return !(a == b);
}

#endif /* canvas_Persistency_Provenance_ProductProvenance_h */

// Local Variables:
// mode: c++
// End:
