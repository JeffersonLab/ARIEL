#include "canvas/Persistency/Provenance/BranchChildren.h"

#include <utility>

namespace art {

  void
  BranchChildren::append_(map_t const& lookup,
                          ProductID const item,
                          ProductIDSet& itemSet) const
  {
    auto const& items = const_cast<map_t&>(lookup)[item];
    // For each parent(child)
    for (auto const& i : items) {
      // Insert the ProductID of the parents (children) into the set
      // of ancestors (descendants).  If the insert succeeds, append
      // recursively.
      if (itemSet.insert(i).second) {
        append_(lookup, i, itemSet);
      }
    }
  }

  void
  BranchChildren::clear()
  {
    childLookup_.clear();
  }

  void
  BranchChildren::insertEmpty(ProductID const parent)
  {
    childLookup_.emplace(parent, ProductIDSet{});
  }

  void
  BranchChildren::insertChild(ProductID const parent, ProductID const child)
  {
    childLookup_[parent].insert(child);
  }

  void
  BranchChildren::appendToDescendants(ProductID const parent,
                                      ProductIDSet& descendants) const
  {
    descendants.insert(parent);
    append_(childLookup_, parent, descendants);
  }
}
