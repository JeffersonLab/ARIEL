#include "canvas/Persistency/Provenance/ProductTables.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/detail/createProductLookups.h"
#include "canvas/Persistency/Provenance/detail/createViewLookups.h"
#include "cetlib/container_algorithms.h"

using namespace art;

namespace {

  auto
  descriptions_for_branch_type(BranchType const bt,
                               ProductDescriptions const& descriptions)
  {
    ProductDescriptionsByID result;
    for (auto const& pd : descriptions) {
      if (pd.branchType() != bt) {
        continue;
      }
      result.try_emplace(pd.productID(), pd);
    }
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

} // unnamed namespace

art::ProductTable::ProductTable(ProductDescriptions const& descs,
                                BranchType const bt)
  : isValid{true}
  , descriptions{descriptions_for_branch_type(bt, descs)}
  , productLookup{detail::createProductLookups(descriptions)}
  , viewLookup{detail::createViewLookups(descriptions)}
{}

cet::exempt_ptr<art::BranchDescription const>
art::ProductTable::description(ProductID const pid) const
{
  if (auto it = descriptions.find(pid); it != cend(descriptions)) {
    return cet::make_exempt_ptr(&it->second);
  }
  return nullptr;
}

art::ProductTables
art::ProductTables::invalid()
{
  return ProductTables{};
}

art::ProductTables::ProductTables(ProductDescriptions const& descriptions)
  : tables_{createProductTables(descriptions)}
{}
