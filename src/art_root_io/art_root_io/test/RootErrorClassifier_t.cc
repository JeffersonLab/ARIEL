#include "art_root_io/detail/RootErrorClassifier.h"

#include "TError.h"

#include <catch2/catch.hpp>
#include <string>

using namespace std::string_literals;
using art::detail::RootErrorClassifier;
using art::detail::RootErrorPayload;

namespace {
  auto
  make_info(std::string const& location, std::string const& message)
  {
    return RootErrorClassifier{kInfo, RootErrorPayload{location, message}};
  }
  auto
  make_error(std::string const& location, std::string const& message)
  {
    return RootErrorClassifier{kError, RootErrorPayload{location, message}};
  }

  enum expected_behavior {
    should_only_suppress,
    should_only_info_log,
    should_only_error_log,
    should_only_throw
  };

  template <expected_behavior expected>
  void
  verify(art::detail::RootErrorClassifier const& classifier)
  {
    if constexpr (expected == should_only_suppress) {
      CHECK(classifier.should_be_suppressed());
    } else {
      CHECK_FALSE(classifier.should_be_suppressed());
    }
    if constexpr (expected == should_only_info_log) {
      CHECK(classifier.should_be_info_logged());
    } else {
      CHECK_FALSE(classifier.should_be_info_logged());
    }
    if constexpr (expected == should_only_error_log) {
      CHECK(classifier.should_be_error_logged());
    } else {
      CHECK_FALSE(classifier.should_be_error_logged());
    }
    if constexpr (expected == should_only_throw) {
      CHECK(classifier.should_be_thrown());
    } else {
      CHECK_FALSE(classifier.should_be_thrown());
    }
  }
}

SCENARIO("Some ROOT messages are fatal errors")
{
  WHEN("Two branches of the same tree have different numbers of entries")
  {
    auto const classifier = make_error(
      "Fill", "attempt to fill branch foo while addresss is not set");
    verify<should_only_throw>(classifier);
  }
  WHEN("There is an error reading a dictionary payload")
  {
    auto const classifier =
      make_error("TCling::AutoParse",
                 "Error parsing payload code for class foo with content:\n"
                 "\n"
                 "#line 1 \"lib_some_dict dictionary payload\"");
    verify<should_only_throw>(classifier);
  }
  WHEN("XRootD is attempting to retry file opens")
  {
    auto const classifier = make_error("TNetXNGFile::Open", "[FATAL]");
    verify<should_only_throw>(classifier);
  }
}

SCENARIO("Some ROOT messages are information")
{
  WHEN("The severity specified for a message is informational")
  {
    auto const classifier =
      make_info("A::B", "Richard Strauss was a German composer");
    verify<should_only_info_log>(classifier);
  }
}

SCENARIO("Some ROOT messages are treated as informational even if they're not")
{
  WHEN("The input file contains obsolete art objects that are not read")
  {
    for (char const* name : {"art::Transient<art::ProductRegistry::Transients>",
                             "art::DoNotRecordParents"}) {
      auto const classifier = make_error(
        "TClass::TClass", "no dictionary for class "s + name + " is available");
      verify<should_only_suppress>(classifier);
    }
  }
  WHEN("Trailing characters while reading data off of branch")
  {
    auto const classifier = make_error(
      "TTree::ReadStream", "Ignoring trailing stuff while reading data");
    verify<should_only_info_log>(classifier);
  }
  WHEN("The message contains the word 'dictionary'")
  {
    auto const classifier = make_error("TBranchElement::Bronch",
                                       "Cannot find dictionary for class foo");
    verify<should_only_info_log>(classifier);
  }
  WHEN("The class is already in the TClassTable")
  {
    auto const classifier =
      make_error("TClassTable::Add", "class foo already in TClassTable");
    verify<should_only_info_log>(classifier);
  }
  WHEN("ROOT has declared a fatal error, but it is due to a pending signal")
  {
    auto const classifier =
      make_error("TUnixSystem::DispatchSignals", "SIGUSR2");
    verify<should_only_error_log>(classifier);
  }
  WHEN("The DISPLAY environment variable is not set")
  {
    auto const classifier =
      make_error("TUnixSystem::SetDisplay", "DISPLAY not set");
    verify<should_only_info_log>(classifier);
  }
  WHEN("XRootD is attempting to retry file opens and authentications")
  {
    AND_WHEN("The TUnixSystem::GetHostByName function is called")
    {
      auto const classifier =
        make_error("TUnixSystem::GetHostByName",
                   "getaddrinfo failed for http://fndca1.fnal.gov/:"
                   "Temporary failure in name resolution");
      verify<should_only_info_log>(classifier);
    }
    AND_WHEN("TNetXNGFile::Open results in a non-fatal error")
    {
      auto const classifier = make_error("TNetXNGFile::Open", "[INFO]");
      verify<should_only_info_log>(classifier);
    }
  }
  WHEN("Any errors in a 'Fit' location are emitted")
  {
    auto const classifier =
      make_error("TF1::SetFitResult", "Invalid Fit result passed");
    verify<should_only_info_log>(classifier);
  }
  WHEN("A Cholesky decomposition error is emitted")
  {
    auto const classifier =
      make_error("TDecompChol::Solve", "matrix is singular");
    verify<should_only_info_log>(classifier);
  }
  WHEN("Any THistPainter::PaintInit errors are emitted")
  {
    auto const classifier =
      make_error("THistPainter::PaintInit", "cannot set x axis to log scale");
    verify<should_only_info_log>(classifier);
  }
  WHEN("Any TGClient::GetFontByName errors are emitted")
  {
    auto const classifier =
      make_error("TGClient::GetFontByName", "couldn't retrieve font");
    verify<should_only_info_log>(classifier);
  }
}
