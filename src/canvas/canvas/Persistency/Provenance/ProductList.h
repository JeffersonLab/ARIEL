#ifndef canvas_Persistency_Provenance_ProductList_h
#define canvas_Persistency_Provenance_ProductList_h
////////////////////////////////////////////////////////////////////////
// ProductList
//
// This is a very-badly-named typedef; please make it go away soon.
////////////////////////////////////////////////////////////////////////

#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/BranchKey.h"
#include "cetlib/container_algorithms.h"
#include <map>

namespace art {
  typedef std::map<BranchKey, BranchDescription> ProductList;

  // FIXME--TEMPORARY INLINE FUNCTION
  inline auto
  make_product_descriptions(ProductList const& productList)
  {
    ProductDescriptions result;
    cet::transform_all(productList, back_inserter(result), [](auto const& pr) {
      return pr.second;
    });
    return result;
  }
}

#endif /* canvas_Persistency_Provenance_ProductList_h */

// Local Variables:
// mode: c++
// End:
