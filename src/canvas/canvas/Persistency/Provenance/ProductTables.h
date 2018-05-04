#ifndef canvas_Persistency_Provenance_ProductTables_h
#define canvas_Persistency_Provenance_ProductTables_h

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/type_aliases.h"

#include <array>

namespace art {

  // A ProductTable is a collection of lookups used in retrieving
  // (views of) products.

  struct ProductTable {
    // A default-constructed ProductTable object represents an invalid
    // table.
    ProductTable() = default;

    explicit ProductTable(ProductDescriptions const& descriptions,
                          BranchType bt);
    explicit ProductTable(ProductDescriptions const& descriptions,
                          BranchType bt,
                          AvailableProducts_t const& availableProducts);

    bool isValid{false};
    ProductLookup_t productLookup{};
    ViewLookup_t viewLookup{};
    AvailableProducts_t availableProducts{};
  };

  // The underlying representation of ProductTables is an array of
  // ProductTable objects: one for each BranchType value.
  class ProductTables {
  public:
    static ProductTables invalid();
    explicit ProductTables(ProductDescriptions const& descriptions);
    explicit ProductTables(
      ProductDescriptions const& descriptions,
      std::array<AvailableProducts_t, NumBranchTypes> const&);

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
    bool isValid_{false};
    std::array<ProductTable, NumBranchTypes> tables_{{}};
  };
}

#endif /* canvas_Persistency_Provenance_ProductTables_h */

// Local Variables:
// mode: c++
// End:
