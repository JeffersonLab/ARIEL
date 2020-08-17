#include "canvas/Persistency/Provenance/Parentage.h"
// vim: set sw=2 expandtab :

#include <ostream>
#include <sstream>
#include <utility>
#include <vector>

using namespace std;

namespace art {

  Parentage::~Parentage() {}

  Parentage::Parentage() : parents_{} {}

  Parentage::Parentage(vector<ProductID> const& parents) : parents_{parents} {}

  Parentage::Parentage(Parentage const& rhs) : parents_{rhs.parents_} {}

  Parentage::Parentage(Parentage&& rhs) : parents_{move(rhs.parents_)} {}

  Parentage&
  Parentage::operator=(Parentage const& rhs)
  {
    if (this != &rhs) {
      parents_ = rhs.parents_;
    }
    return *this;
  }

  Parentage&
  Parentage::operator=(Parentage&& rhs)
  {
    parents_ = move(rhs.parents_);
    return *this;
  }

  ParentageID
  Parentage::id() const
  {
    ostringstream oss;
    for (auto const pid : parents_) {
      oss << pid << ' ';
    }
    string const stringrep{oss.str()};
    return ParentageID{cet::MD5Digest{stringrep}.digest().toString()};
  }

  vector<ProductID> const&
  Parentage::parents() const
  {
    return parents_;
  }

  ostream&
  operator<<(ostream& os, Parentage const&)
  {
    // Unimplemented
    return os;
  }

  bool
  operator==(Parentage const& a, Parentage const& b)
  {
    return a.parents() == b.parents();
  }

  bool
  operator!=(Parentage const& a, Parentage const& b)
  {
    return !(a == b);
  }

} // namespace art
