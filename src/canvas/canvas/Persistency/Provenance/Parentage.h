#ifndef canvas_Persistency_Provenance_Parentage_h
#define canvas_Persistency_Provenance_Parentage_h

/*----------------------------------------------------------------------

Parentage: The products that were read in producing this product.

definitions:
Product: The EDProduct to which a provenance object is associated
Parents: The EDProducts used as input by the creator.

----------------------------------------------------------------------*/

#include "canvas/Persistency/Provenance/ParentageID.h"
#include "canvas/Persistency/Provenance/ProductID.h"

#include <iosfwd>
#include <vector>

namespace art {
  class Parentage {
  public:
    Parentage() = default;
    explicit Parentage(std::vector<ProductID> const& parents);

    // Only the 'salient attributes' are encoded into the ID.
    ParentageID id() const;

    std::vector<ProductID> const&
    parents() const
    {
      return parents_;
    }
    std::vector<ProductID>&
    parents()
    {
      return parents_;
    }

  private:
    std::vector<ProductID> parents_{};
  };

  inline std::ostream&
  operator<<(std::ostream& os, Parentage const&)
  {
    // Unimplemented
    return os;
  }

  // Only the 'salient attributes' are testing in equality comparison.
  bool operator==(Parentage const& a, Parentage const& b);
  inline bool
  operator!=(Parentage const& a, Parentage const& b)
  {
    return !(a == b);
  }
}
#endif /* canvas_Persistency_Provenance_Parentage_h */

// Local Variables:
// mode: c++
// End:
