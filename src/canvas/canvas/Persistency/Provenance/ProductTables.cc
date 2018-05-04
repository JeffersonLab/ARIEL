#include "canvas/Persistency/Provenance/ProductTables.h"
#include "canvas/Persistency/Provenance/detail/createProductLookups.h"
#include "canvas/Persistency/Provenance/detail/createViewLookups.h"
#include "cetlib/container_algorithms.h"

using namespace art;

namespace {
  auto
  descriptions_for_branch_type(BranchType const bt,
                               ProductDescriptions const& descriptions)
  {
    ProductDescriptions result;
    cet::copy_if_all(descriptions, back_inserter(result), [bt](auto const& pd) {
      return pd.branchType() == bt;
    });
    return result;
  }

  auto
  available_products(ProductDescriptions const& descriptions)
  {
    AvailableProducts_t result;
    cet::transform_all(descriptions,
                       inserter(result, begin(result)),
                       [](auto const& pd) { return pd.productID(); });
    return result;
  }

  using ProductTables_t = std::array<ProductTable, NumBranchTypes>;
  auto
  createProductTables(ProductDescriptions const& descriptions)
  {
    ProductTables_t result{{}};
    for (std::size_t bt{}; bt < NumBranchTypes; ++bt) {
      result[bt] = ProductTable{descriptions, static_cast<BranchType>(bt)};
    }
    return result;
  }

  auto
  createProductTables(
    ProductDescriptions const& descriptions,
    std::array<AvailableProducts_t, NumBranchTypes> const& availableProducts)
  {
    ProductTables_t result{{}};
    for (std::size_t bt{}; bt < NumBranchTypes; ++bt) {
      result[bt] = ProductTable{
        descriptions, static_cast<BranchType>(bt), availableProducts[bt]};
    }
    return result;
  }
}

// =======================================================================

art::ProductTable::ProductTable(ProductDescriptions const& descriptions,
                                BranchType const bt)
  : ProductTable{
      descriptions,
      bt,
      available_products(descriptions_for_branch_type(bt, descriptions))}
{}

art::ProductTable::ProductTable(ProductDescriptions const& descriptions,
                                BranchType const bt,
                                AvailableProducts_t const& availableProducts)
  : isValid{true}
  , productLookup{detail::createProductLookups(
      descriptions_for_branch_type(bt, descriptions))}
  , viewLookup{detail::createViewLookups(
      descriptions_for_branch_type(bt, descriptions))}
  , availableProducts{availableProducts}
{}

art::ProductTables
art::ProductTables::invalid()
{
  return ProductTables{};
}

art::ProductTables::ProductTables(ProductDescriptions const& descriptions)
  : tables_{createProductTables(descriptions)}
{}

art::ProductTables::ProductTables(
  ProductDescriptions const& descriptions,
  std::array<AvailableProducts_t, NumBranchTypes> const& availableProducts)
  : tables_{createProductTables(descriptions, availableProducts)}
{}
