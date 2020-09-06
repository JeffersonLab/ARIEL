#include "canvas/Persistency/Common/RNGsnapshot.h"
// vim: set sw=2 expandtab :

#include "cetlib/container_algorithms.h"

#include <string>
#include <vector>

using namespace std;

namespace art {

  RNGsnapshot::RNGsnapshot(std::string const& ekind,
                           std::string const& lbl,
                           std::vector<unsigned long> const& est)
  {
    saveFrom(ekind, lbl, est);
  }

  void
  RNGsnapshot::saveFrom(std::string const& ekind,
                        std::string const& lbl,
                        std::vector<unsigned long> const& est)
  {
    engine_kind_ = ekind;
    label_ = lbl;
    // N.B. implicit conversion from unsigned long to unsigned int.
    cet::copy_all(est, std::back_inserter(state_));
  }

  std::vector<unsigned long>
  RNGsnapshot::restoreState() const
  {
    std::vector<unsigned long> est;
    // N.B. implicit conversion from unsigned long to unsigned int.
    cet::copy_all(state_, std::back_inserter(est));
    return est;
  }

} // namespace art
