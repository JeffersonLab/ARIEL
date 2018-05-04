#ifndef canvas_Persistency_Provenance_BranchKey_h
#define canvas_Persistency_Provenance_BranchKey_h

/*----------------------------------------------------------------------

BranchKey: The key used to identify a Group in the EventPrincipal. The
name of the branch to which the related data product will be written
is determined entirely from the BranchKey.

----------------------------------------------------------------------*/

#include "canvas/Persistency/Provenance/BranchType.h"

#include <iosfwd>
#include <string>
#include <tuple>

namespace art {
  class BranchDescription;

  struct BranchKey;

  bool operator<(BranchKey const& a, BranchKey const& b);
  bool operator==(BranchKey const& a, BranchKey const& b);
  bool operator!=(BranchKey const& a, BranchKey const& b);

  std::ostream& operator<<(std::ostream& os, BranchKey const& bk);
}

struct art::BranchKey {
  BranchKey() = default;
  BranchKey(std::string cn,
            std::string ml,
            std::string pin,
            std::string pn,
            BranchType bt = NumBranchTypes);

  explicit BranchKey(BranchDescription const& desc);

  std::string branchName() const;

  std::string friendlyClassName_{};
  std::string moduleLabel_{};
  std::string productInstanceName_{};
  std::string processName_{};
  int branchType_{NumBranchTypes};
};

inline art::BranchKey::BranchKey(std::string cn,
                                 std::string ml,
                                 std::string pin,
                                 std::string pn,
                                 BranchType const bt)
  : friendlyClassName_{std::move(cn)}
  , moduleLabel_{std::move(ml)}
  , productInstanceName_{std::move(pin)}
  , processName_{std::move(pn)}
  , branchType_{bt}
{}

inline bool
art::operator<(BranchKey const& a, BranchKey const& b)
{
  return std::tie(a.friendlyClassName_,
                  a.moduleLabel_,
                  a.productInstanceName_,
                  a.processName_,
                  a.branchType_) < std::tie(b.friendlyClassName_,
                                            b.moduleLabel_,
                                            b.productInstanceName_,
                                            b.processName_,
                                            b.branchType_);
}

inline bool
art::operator==(BranchKey const& a, BranchKey const& b)
{
  return !(a < b || b < a);
}

inline bool
art::operator!=(BranchKey const& a, BranchKey const& b)
{
  return !(a == b);
}

#endif /* canvas_Persistency_Provenance_BranchKey_h */

// Local Variables:
// mode: c++
// End:
