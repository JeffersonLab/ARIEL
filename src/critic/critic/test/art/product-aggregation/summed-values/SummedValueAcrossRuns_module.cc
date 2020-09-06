#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/SharedAnalyzer.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SummedValue.h"

#include "art/test/TestObjects/ToyProducts.h"

namespace art::test {
  class SummedValueAcrossRuns : public SharedAnalyzer {
  public:
    struct Config {
      fhicl::Atom<InputTag> inputTag{
        fhicl::Name{"inputTag"},
        fhicl::Comment{"Input tag for upstream module that produced\n"
                       "the 'arttest::StringProduct' run product"}};
    };
    using Parameters = Table<Config>;
    explicit SummedValueAcrossRuns(Parameters const& p, ProcessingFrame const&);

  private:
    void beginRun(Run const& r, ProcessingFrame const&) override;
    void
    analyze(Event const&, ProcessingFrame const&) override
    {}
    ProductToken<arttest::StringProduct> token_;
    SummedValue<arttest::StringProduct> summed_;
    bool firstRunAlreadySeen_{false};
  };

  SummedValueAcrossRuns::SummedValueAcrossRuns(Parameters const& p,
                                               ProcessingFrame const&)
    : SharedAnalyzer{p}
    , token_{consumes<arttest::StringProduct>(p().inputTag())}
  {
    async<art::InEvent>();
  }

  void
  SummedValueAcrossRuns::beginRun(Run const& r, ProcessingFrame const&)
  {
    auto const h = r.getValidHandle(token_);
    try {
      summed_.update(h);
      firstRunAlreadySeen_ = true;
    }
    catch (cet::exception const& e) {
      assert(firstRunAlreadySeen_);
      assert(e.explain_self().find("have different run numbers") !=
             std::string::npos);
    }
  }

}

DEFINE_ART_MODULE(art::test::SummedValueAcrossRuns)
