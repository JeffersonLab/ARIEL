#include "art/Framework/Core/InputSourceMutex.h"
#include "art_root_io/Inputfwd.h"
// vim: set sw=2 expandtab :

#include "canvas/Utilities/Exception.h"
#include "hep_concurrency/tsan.h"

#include "TBranch.h"
#include "TTree.h"

#include <iostream>

using namespace std;
using namespace hep::concurrency;

namespace art {
  namespace input {

    Int_t
    getEntry(TBranch* branch, EntryNumber entryNumber)
    {
      InputSourceMutexSentry sentry;
      try {
        return branch->GetEntry(entryNumber);
      }
      catch (cet::exception& e) {
        throw art::Exception(art::errors::FileReadError)
          << e.explain_self() << "\n";
      }
    }

    Int_t
    getEntry(TBranch* branch,
             EntryNumber entryNumber,
             unsigned long long& ticks [[maybe_unused]])
    {
      return getEntry(branch, entryNumber);
    }

    Int_t
    getEntry(TTree* tree, EntryNumber entryNumber)
    {
      InputSourceMutexSentry sentry;
      try {
        return tree->GetEntry(entryNumber);
      }
      catch (cet::exception& e) {
        throw art::Exception(art::errors::FileReadError)
          << e.explain_self() << "\n";
      }
    }

    Int_t
    getEntry(TTree* tree,
             EntryNumber entryNumber,
             unsigned long long& ticks [[maybe_unused]])
    {
      return getEntry(tree, entryNumber);
    }

  } // namespace input
} // namespace art
