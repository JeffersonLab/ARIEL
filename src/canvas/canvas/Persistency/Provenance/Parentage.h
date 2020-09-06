#ifndef canvas_Persistency_Provenance_Parentage_h
#define canvas_Persistency_Provenance_Parentage_h
// vim: set sw=2 expandtab :

//  The products that were read in producing this product.

#include "canvas/Persistency/Provenance/ParentageID.h"
#include "canvas/Persistency/Provenance/ProductID.h"

#include <iosfwd>
#include <vector>

namespace art {

  class Parentage {

  public:
    ~Parentage();

    Parentage();

    explicit Parentage(std::vector<ProductID> const& parents);

    Parentage(Parentage const&);

    Parentage(Parentage&&);

    Parentage& operator=(Parentage const&);

    Parentage& operator=(Parentage&&);

  public:
    ParentageID id() const;

    std::vector<ProductID> const& parents() const;

  private:
    std::vector<ProductID> parents_{};
  };

  std::ostream& operator<<(std::ostream& os, Parentage const&);

  bool operator==(Parentage const& a, Parentage const& b);

  bool operator!=(Parentage const& a, Parentage const& b);

} // namespace art

#endif /* canvas_Persistency_Provenance_Parentage_h */

// Local Variables:
// mode: c++
// End:
