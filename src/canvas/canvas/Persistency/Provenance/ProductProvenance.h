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
  typedef std::vector<ProductProvenance> ProductProvenances;

  bool operator<(ProductProvenance const& a, ProductProvenance const& b);
  std::ostream& operator<<(std::ostream& os, ProductProvenance const& p);
  bool operator==(ProductProvenance const& a, ProductProvenance const& b);
  bool operator!=(ProductProvenance const& a, ProductProvenance const& b);
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
  productID() const
  {
    return productID_;
  }
  ProductStatus const&
  productStatus() const
  {
    return productStatus_;
  }
  ParentageID const&
  parentageID() const
  {
    return parentageID_;
  }
  Parentage const& parentage() const;
  void
  setStatus(ProductStatus status) const
  {
    productStatus_ = status;
  }
  void setPresent() const;
  void setNotPresent() const;

  bool
  noParentage() const
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
  parentagePtr() const
  {
    return transients_.get().parentagePtr_;
  }

  ProductID productID_{};
  mutable ProductStatus productStatus_{productstatus::uninitialized()};
  ParentageID parentageID_{};
  mutable Transient<Transients> transients_{};
};

inline bool
art::operator<(ProductProvenance const& a, ProductProvenance const& b)
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
art::operator!=(ProductProvenance const& a, ProductProvenance const& b)
{
  return !(a == b);
}

#endif /* canvas_Persistency_Provenance_ProductProvenance_h */

// Local Variables:
// mode: c++
// End:
