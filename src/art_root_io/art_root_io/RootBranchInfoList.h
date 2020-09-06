#ifndef art_root_io_RootBranchInfoList_h
#define art_root_io_RootBranchInfoList_h

#include "art/Utilities/fwd.h"
#include "art_root_io/RootBranchInfo.h"
#include "canvas/Utilities/TypeID.h"

#include <vector>

#include "TTree.h"

namespace art {
  class RootBranchInfoList;
}

class art::RootBranchInfoList {
public:
  RootBranchInfoList();
  explicit RootBranchInfoList(TTree* tree);
  void reset(TTree* tree);

  template <class PROD>
  bool findBranchInfo(InputTag const& tag, RootBranchInfo& rbInfo) const;

  bool findBranchInfo(TypeID const& type,
                      InputTag const& tag,
                      RootBranchInfo& rbInfo) const;

private:
  std::vector<RootBranchInfo> data_{};
};

template <class PROD>
bool
art::RootBranchInfoList::findBranchInfo(InputTag const& tag,
                                        RootBranchInfo& rbInfo) const
{
  return findBranchInfo(TypeID{typeid(PROD)}, tag, rbInfo);
}

#endif /* art_root_io_RootBranchInfoList_h */

// Local Variables:
// mode: c++
// End:
