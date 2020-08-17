#include "art_root_io/setup.h"
#include "art_root_io/RootDB/tkeyvfs.h"
#include "art_root_io/detail/RootErrorClassifier.h"
#include "canvas/Utilities/Exception.h"
#include "canvas_root_io/Streamers/BranchDescriptionStreamer.h"
#include "canvas_root_io/Streamers/CacheStreamers.h"
#include "canvas_root_io/Streamers/ProductIDStreamer.h"
#include "canvas_root_io/Streamers/RefCoreStreamer.h"
#include "canvas_root_io/Streamers/TransientStreamer.h"
#include "canvas_root_io/Streamers/setPtrVectorBaseStreamer.h"
#include "hep_concurrency/RecursiveMutex.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "TError.h"
#include "TH1.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"

#include <regex>
#include <sstream>
#include <string.h>

using namespace hep::concurrency;
using namespace std::string_literals;
using art::detail::RootErrorClassifier;
using art::detail::RootErrorPayload;

namespace {

  inline std::ostream&
  operator<<(std::ostream& os, RootErrorPayload const& payload)
  {
    return os << payload.location << '\n' << payload.message;
  }

  // The prefix '@SUB=' is used to specify the subroutine in which a
  // messagefacility message is emitted.
  inline mf::LogInfo&&
  operator<<(mf::LogInfo&& logger, RootErrorPayload const& payload)
  {
    return logger << "@SUB=" + payload.location << payload.message;
  }
  inline mf::LogError&&
  operator<<(mf::LogError&& logger, RootErrorPayload const& payload)
  {
    return logger << "@SUB=" + payload.location << payload.message;
  }

  void
  RootErrorHandler(int const level,
                   bool /*abort*/,
                   char const* location,
                   char const* message) noexcept(false)
  {
    auto str_or_question_mark = [](char const* str) {
      return std::string{str ? str : "?"};
    };
    RootErrorPayload const payload{str_or_question_mark(location),
                                   str_or_question_mark(message)};
    RootErrorClassifier const classifier{level, payload};

    if (classifier.should_be_suppressed()) {
      return;
    }

    if (classifier.should_be_info_logged()) {
      mf::LogInfo("Root_Information")
        << payload << "\nROOT severity: " << level;
      return;
    }

    if (classifier.should_be_error_logged()) {
      mf::LogError("Root_Error") << payload << "\nROOT severity: " << level;
      return;
    }

    std::ostringstream sstr;
    sstr << "Fatal Root Error: " << payload << "\nROOT severity: " << level
         << '\n';
    if (classifier.should_be_thrown()) {
      throw art::Exception{art::errors::FatalRootError, sstr.str()};
    }

    // Shouldn't get here
    throw art::Exception{art::errors::LogicError, sstr.str()}
      << "Please report this logic error to artists@fnal.gov\n";
  }

  RecursiveMutex mutex{"art::root::setup"};
  bool initialized{false};

} // namespace

void
art::root::setup()
{
  RecursiveMutexSentry lock{mutex};
  if (initialized) {
    return;
  }

  // Set error handler
  gSystem->ResetSignals();
  SetErrorHandler(RootErrorHandler);

  // Set ROOT parameters.  See note in EventProcessor about when
  // EnableImplicitMT() should be called.
  ROOT::EnableThreadSafety();
  TTree::SetMaxTreeSize(kMaxLong64);
  TH1::AddDirectory(kFALSE);

  // Initialize tkeyvfs sqlite3 extension for ROOT.
  tkeyvfs_init();

  // Set custom streamers.
  setCacheStreamers();
  setProvenanceTransientStreamers();
  detail::setBranchDescriptionStreamer();
  detail::setPtrVectorBaseStreamer();
  configureProductIDStreamer();
  configureRefCoreStreamer();
  initialized = true;
}
