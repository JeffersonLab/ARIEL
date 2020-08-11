#include "canvas/Persistency/Provenance/Parentage.h"

#include <ostream>
#include <sstream>

namespace art {

  Parentage::Parentage(std::vector<ProductID> const& parents)
    : parents_{parents}
  {}

  ParentageID
  Parentage::id() const
  {
    // This implementation is ripe for optimization.
    std::ostringstream oss;
    for (auto const& pid : parents_) {
      oss << pid << ' ';
    }

    std::string const stringrep{oss.str()};
    return ParentageID{cet::MD5Digest{stringrep}.digest().toString()};
  }

  bool
  operator==(Parentage const& a, Parentage const& b)
  {
    return a.parents() == b.parents();
  }
}
