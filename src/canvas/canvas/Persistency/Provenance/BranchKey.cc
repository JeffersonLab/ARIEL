#include "canvas/Persistency/Provenance/BranchKey.h"
#include "canvas/Persistency/Provenance/BranchDescription.h"
#include "canvas/Persistency/Provenance/canonicalProductName.h"

#include <ostream>

namespace {
  constexpr char underscore [[maybe_unused]]{'_'};
  constexpr char period [[maybe_unused]]{'.'};
}

namespace art {

  BranchKey::BranchKey(BranchDescription const& desc)
    : friendlyClassName_{desc.friendlyClassName()}
    , moduleLabel_{desc.moduleLabel()}
    , productInstanceName_{desc.productInstanceName()}
    , processName_{desc.processName()}
    , branchType_{desc.branchType()}
  {}

  std::string
  BranchKey::branchName() const
  {
    return canonicalProductName(
      friendlyClassName_, moduleLabel_, productInstanceName_, processName_);
  }

  std::ostream&
  operator<<(std::ostream& os, BranchKey const& bk)
  {
    os << "BranchKey(" << bk.friendlyClassName_ << ", " << bk.moduleLabel_
       << ", " << bk.productInstanceName_ << ", " << bk.processName_ << ", "
       << static_cast<BranchType>(bk.branchType_) << ')';
    return os;
  }
}
