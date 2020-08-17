#include "art_root_io/detail/RootErrorClassifier.h"

#include "TError.h"

#include <regex>
#include <sstream>

using namespace std::string_literals;
using art::detail::RootErrorParser;
using art::detail::RootErrorPayload;

namespace {
  bool
  can_suppress_error(RootErrorPayload const& payload)
  {
    RootErrorParser const parser{payload};
    // Suppress errors regarding obsolete art objects
    for (std::string const cls :
         {"art::Transient<art::ProductRegistry::Transients>",
          "art::DoNotRecordParents"}) {
      if (parser.has_message("no dictionary for class "s + cls +
                             " is available"s)) {
        return true;
      }
    }
    return false;
  }

  bool
  can_info_log(int const level, RootErrorPayload const& payload)
  {
    RootErrorParser const parser{payload};
    // Print an informational message if (a) the severity is at the
    // "info" level, or (b) if the art users have asked a specific
    // error to not result in an exception throw.
    if (level == kInfo) {
      return true;
    }

    if (parser.has_any_of_these_messages(
          "already in TClassTable",
          "matrix not positive definite",
          "number of iterations was insufficient",
          "bad integrand behavior")) {
      return true;
    }

    if (parser.has_message("dictionary") and
        not parser.has_message("Error parsing payload code for class")) {
      return true;
    }

    if (parser.in_any_of_these_locations("Fit",
                                         "TDecompChol::Solve",
                                         "THistPainter::PaintInit",
                                         "TGClient::GetFontByName",
                                         "TUnixSystem::GetHostByName")) {
      return true;
    }

    if (parser.has_message_in("Ignoring trailing", "TTree::ReadStream")) {
      return true;
    }

    if (parser.has_message_in("DISPLAY not set", "TUnixSystem::SetDisplay")) {
      return true;
    }

    // Necessary for enabling XRootD retries
    if (parser.in("TNetXNGFile::Open") and not parser.has_message("[FATAL]")) {
      return true;
    }

    return false;
  }

  bool
  can_error_log(RootErrorPayload const& payload)
  {
    RootErrorParser const parser{payload};
    // For pending signals, do not throw but let the OS deal with the
    // signal in the usual way.
    if (parser.in("TUnixSystem::DispatchSignals")) {
      return true;
    }
    return false;
  }

}

namespace art::detail {

  RootErrorClassifier::RootErrorClassifier(int const level,
                                           RootErrorPayload const& payload)
    : canSuppress_{can_suppress_error(payload)}
    , canInfoLog_{can_info_log(level, payload)}
    , canErrorLog_{can_error_log(payload)}
  {}

} // art::detail
