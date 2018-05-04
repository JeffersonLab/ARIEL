#ifndef canvas_Persistency_Provenance_BranchChildren_h
#define canvas_Persistency_Provenance_BranchChildren_h

/*----------------------------------------------------------------------

BranchChildren: Dependency information between products.

----------------------------------------------------------------------*/

#include "canvas/Persistency/Provenance/ProductID.h"

#include <map>
#include <set>

namespace art {

  class BranchChildren {
  private:
    using ProductIDSet = std::set<ProductID>;

  public:
    // Clear all information.
    void clear();

    // Insert a parent with no children.
    void insertEmpty(ProductID parent);

    // Insert a new child for the given parent.
    void insertChild(ProductID parent, ProductID child);

    // Look up all the descendants of the given parent, and insert
    // them into descendants. N.B.: this does not clear out
    // descendants first; it only appends *new* elements to the
    // collection.
    void appendToDescendants(ProductID parent, ProductIDSet& descendants) const;

    // Public type alias to facilitate ROOT IO rule.
    using map_t = std::map<ProductID, ProductIDSet>;

  private:
    map_t childLookup_;
    void append_(map_t const& lookup,
                 ProductID item,
                 ProductIDSet& itemSet) const;
  };
}
#endif /* canvas_Persistency_Provenance_BranchChildren_h */

// Local Variables:
// mode: c++
// End:
