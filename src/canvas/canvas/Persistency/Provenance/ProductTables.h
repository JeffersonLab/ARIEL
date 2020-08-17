#ifndef canvas_Persistency_Provenance_ProductTables_h
#define canvas_Persistency_Provenance_ProductTables_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/type_aliases.h"
#include "cetlib/exempt_ptr.h"

#include <array>

namespace art {

  // A ProductTable is a collection of lookups used in retrieving
  // (views of) products.

  struct ProductTable {

    // A default-constructed ProductTable object represents an invalid table.
    ProductTable() = default;
    explicit ProductTable(ProductDescriptions const& descriptions,
                          BranchType bt);

    cet::exempt_ptr<BranchDescription const> description(ProductID) const;
    bool isValid{false};
    ProductDescriptionsByID descriptions{};
    ProductLookup_t productLookup{};
    ViewLookup_t viewLookup{};
  };

  // The underlying representation of ProductTables is an array of
  // ProductTable objects: one for each BranchType value.
  class ProductTables {

  public:
    explicit ProductTables(ProductDescriptions const& descriptions);

  public:
    static ProductTables invalid();

    auto const&
    descriptions(BranchType const bt) const
    {
      return tables_[bt].descriptions;
    }

    auto&
    get(BranchType const bt)
    {
      return tables_[bt];
    }

    auto const&
    get(BranchType const bt) const
    {
      return tables_[bt];
    }
    bool
    isValid() const
    {
      return isValid_;
    }

  private:
    explicit ProductTables() = default;

  private:
    bool isValid_{false};
    std::array<ProductTable, NumBranchTypes> tables_{{}};
  };

} // namespace art

#endif /* canvas_Persistency_Provenance_ProductTables_h */

// Local Variables:
// mode: c++
// End:
