#ifndef art_root_io_RootDelayedReader_h
#define art_root_io_RootDelayedReader_h
// vim: set sw=2 expandtab :

#include "art/Persistency/Common/DelayedReader.h"
#include "art_root_io/Inputfwd.h"
#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/Compatibility/BranchIDList.h"
#include "canvas/Persistency/Provenance/EventID.h"
#include "canvas/Persistency/Provenance/FileFormatVersion.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/RangeSet.h"

#include <atomic>
#include <map>
#include <memory>
#include <string>

extern "C" {
#include "sqlite3.h"
} // extern "C"

class TBranch;
class TFile;

namespace art {
  class Group;
  class Principal;
  class ProductProvenance;
  class RootInputFile;
  class RootInputTree;
  class RootDelayedReader final : public DelayedReader {
  public: // MEMBER FUNCTIONS -- Special Member Functions
    ~RootDelayedReader();
    RootDelayedReader(RootDelayedReader const&) = delete;
    RootDelayedReader& operator=(RootDelayedReader const&) = delete;
    RootDelayedReader(RootDelayedReader&&) = delete;
    RootDelayedReader& operator=(RootDelayedReader&&) = delete;
    RootDelayedReader(FileFormatVersion,
                      sqlite3* db,
                      std::vector<input::EntryNumber> const& entrySet,
                      cet::exempt_ptr<input::BranchMap const>,
                      TBranch* provenanceBranch,
                      int64_t saveMemoryObjectThreshold,
                      cet::exempt_ptr<RootInputFile> primaryFile,
                      cet::exempt_ptr<BranchIDLists const> branchIDLists,
                      BranchType branchType,
                      EventID,
                      bool compactSubRunRanges);

  private: // MEMBER FUNCTIONS -- API
    std::unique_ptr<EDProduct> getProduct_(Group const*,
                                           ProductID,
                                           RangeSet&) const override;
    void setPrincipal_(cet::exempt_ptr<Principal>) override;
    std::vector<ProductProvenance> readProvenance_() const override;
    bool isAvailableAfterCombine_(ProductID) const override;
    int openNextSecondaryFile_(int idx) override;

  private: // MEMBER DATA -- Implementation details.
    FileFormatVersion fileFormatVersion_;
    sqlite3* db_;
    std::vector<input::EntryNumber> const entrySet_;
    cet::exempt_ptr<input::BranchMap const> branches_;
    TBranch* provenanceBranch_;
    int64_t saveMemoryObjectThreshold_;
    cet::exempt_ptr<Principal> principal_;
    cet::exempt_ptr<RootInputFile> primaryFile_;
    cet::exempt_ptr<BranchIDLists const> branchIDLists_;
    BranchType branchType_;
    EventID eventID_;
    bool const compactSubRunRanges_;
  };
} // namespace art

// Local Variables:
// mode: c++
// End:
#endif /* art_root_io_RootDelayedReader_h */
