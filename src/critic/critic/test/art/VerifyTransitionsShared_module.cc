#include "cetlib/quiet_unit_test.hpp"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/SharedProducer.h"
#include "art/Framework/Principal/fwd.h"
#include "art/Utilities/Globals.h"

#include <atomic>

namespace {
  class VerifyTransitions : public art::SharedProducer {
  public:
    struct Config {};
    using Parameters = Table<Config>;
    VerifyTransitions(Parameters const& ps, art::ProcessingFrame const&)
      : SharedProducer{ps}
    {
      if (art::Globals::instance()->nschedules() == 1) {
        throw art::Exception{art::errors::Configuration}
          << "This module is intended to be used with more than one schedule.  "
             "Please reconfigure your job.\n";
      }
      async<art::InEvent>();
    }

  private:
    std::atomic<unsigned> nBRcalls_{};
    std::atomic<unsigned> nBSRcalls_{};
    std::atomic<unsigned> nESRcalls_{};
    std::atomic<unsigned> nERcalls_{};

    void
    beginRun(art::Run&, art::ProcessingFrame const&) override
    {
      ++nBRcalls_;
    }
    void
    beginSubRun(art::SubRun&, art::ProcessingFrame const&) override
    {
      ++nBSRcalls_;
    }
    void
    produce(art::Event&, art::ProcessingFrame const&) override
    {}
    void
    endSubRun(art::SubRun&, art::ProcessingFrame const&) override
    {
      ++nESRcalls_;
    }
    void
    endRun(art::Run&, art::ProcessingFrame const&) override
    {
      ++nERcalls_;
    }

    void
    endJob(art::ProcessingFrame const&) override
    {
      BOOST_TEST(nBRcalls_ == 1u);
      BOOST_TEST(nBSRcalls_ == 1u);
      BOOST_TEST(nESRcalls_ == 1u);
      BOOST_TEST(nERcalls_ == 1u);
    }
  };
}

DEFINE_ART_MODULE(VerifyTransitions)
