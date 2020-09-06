// vim: set sw=2 expandtab :

#include "canvas_root_io/Streamers/ProductIDStreamer.h"
#include "canvas/Persistency/Provenance/Compatibility/type_aliases.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/Exception.h"

#include "TBuffer.h"
#include "TClassRef.h"

namespace {

  // See comments below regarding how the translation is formed.
  auto
  form_translation_map(cet::exempt_ptr<art::BranchIDLists const> branchIDLists)
  {
    std::vector<std::size_t> result;
    if (!branchIDLists) {
      return result;
    }

    auto const& lists = *branchIDLists;
    for (std::size_t i{}; i < lists.size(); ++i) {
      if (lists[i].empty())
        continue;
      result.push_back(i);
    }
    return result;
  }
}

namespace art {

  ProductIDStreamer::ProductIDStreamer(
    cet::exempt_ptr<BranchIDLists const> branchIDLists)
    : branchIDLists_{branchIDLists}
    , branchIDListsIndices_{form_translation_map(branchIDLists_)}
  {}

  void
  ProductIDStreamer::setBranchIDLists(
    cet::exempt_ptr<BranchIDLists const> bidLists)
  {
    ProductIDStreamer tmp{bidLists};
    std::swap(*this, tmp);
  }

  TClassStreamer*
  ProductIDStreamer::Generate() const
  {
    return new ProductIDStreamer{*this};
  }

  void
  ProductIDStreamer::operator()(TBuffer& R_b, void* objp)
  {
    static TClassRef cl{"art::ProductID"};
    if (R_b.IsReading()) {
      UInt_t start, count;
      auto version = R_b.ReadVersion(&start, &count);
      if (version >= 10) {
        cl->ReadBuffer(R_b, objp, version, start, count);
        return;
      }

      // Extract the data members from the old version of the ProductID
      compatibility::ProcessIndex oldProcessIndex;
      compatibility::ProductIndex oldProductIndex;
      R_b >> oldProcessIndex >> oldProductIndex;

      auto obj = static_cast<ProductID*>(objp);
      // It is okay to return early without setting obj->value_ since
      // it defaults to an invalid value.

      if (branchIDLists_) {
        // The processIndex_ and productIndex_ values are indexed
        // starting at 1, not 0.
        if (oldProcessIndex == 0 || oldProductIndex == 0)
          return;

        // The process index is even more fiddly: in addition to
        // indexing starting at 1, it is an index into the
        // BranchListIndexes list of the current event (which is no
        // longer stored).  To translate to the correct BranchIDLists
        // index, we should technically load the history for each
        // event.  However, since we guarantee that process histories
        // for older versions of art cannot be inconsistent, it is
        // sufficient to provide a global translation from the process
        // index to the BranchIDLists index.  The translation is that
        // all empty BranchIDLists entries are not included in the
        // BranchListIndexes list--i.e. we merely need to skip each
        // process with an empty BranchIDList.
        auto const& data = *branchIDLists_;
        auto const oldBIDListsIndex =
          branchIDListsIndices_[oldProcessIndex - 1];
        if (oldProcessIndex <= data.size() &&
            oldProductIndex <= data[oldBIDListsIndex].size()) {
          obj->value_ = data[oldBIDListsIndex][oldProductIndex - 1];
        }
      } else {
        throw Exception{errors::DataCorruption, "ProductID streamer:\n"}
          << "BranchIDLists not available for converting from obsolete "
             "ProductID schema to current one.\n";
      }
    } else {
      cl->WriteBuffer(R_b, objp);
    }
  }

  void
  configureProductIDStreamer(cet::exempt_ptr<BranchIDLists const> bidLists)
  {
    static TClassRef cl{"art::ProductID"};
    auto st = static_cast<ProductIDStreamer*>(cl->GetStreamer());
    if (st == nullptr) {
      cl->AdoptStreamer(new ProductIDStreamer{bidLists});
    } else {
      st->setBranchIDLists(bidLists);
    }
  }
}
