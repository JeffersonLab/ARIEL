#ifndef canvas_Persistency_Provenance_BranchMapper_h
#define canvas_Persistency_Provenance_BranchMapper_h

// ======================================================================
//
// BranchMapper: Manages the per event/subRun/run per product provenance.
//
// ======================================================================

#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProductProvenance.h"
#include "cetlib/container_algorithms.h"
#include "cetlib/exempt_ptr.h"
#include "cetlib/value_ptr.h"

#include <iosfwd>
#include <map>
#include <memory>
#include <set>

namespace art {
  // defined below:
  class BranchMapper;
  std::ostream& operator<<(std::ostream&, BranchMapper const&);

  // forward declaration:
  class ProductID;
}

// ----------------------------------------------------------------------

class art::BranchMapper {
public:
  BranchMapper(BranchMapper const&) = delete;
  BranchMapper& operator=(BranchMapper const&) = delete;

  using result_t = cet::exempt_ptr<ProductProvenance const>;

  explicit BranchMapper(bool delayedRead = false);
  virtual ~BranchMapper() = default;

  void write(std::ostream&) const;

  result_t branchToProductProvenance(ProductID const) const;
  result_t insert(std::unique_ptr<ProductProvenance const>&&);

  void
  setDelayedRead(bool const value)
  {
    delayedRead_ = value;
  }

private:
  using eiSet = std::map<ProductID, cet::value_ptr<ProductProvenance const>>;

  eiSet entryInfoSet_{};
  mutable bool delayedRead_;

  void readProvenance() const;
  virtual void
  readProvenance_() const
  {}

}; // BranchMapper

inline std::ostream&
art::operator<<(std::ostream& os, BranchMapper const& p)
{
  p.write(os);
  return os;
}

  // ======================================================================

#endif /* canvas_Persistency_Provenance_BranchMapper_h */

// Local Variables:
// mode: c++
// End:
