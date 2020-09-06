#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art/Utilities/make_tool.h"
#include "critic/test/art/ReadNextArbitrator.h"

#include <cassert>

namespace art::test {
  class GeneratorTestDetail;
}

class art::test::GeneratorTestDetail {
public:
  GeneratorTestDetail(GeneratorTestDetail const&) = delete;
  GeneratorTestDetail& operator=(GeneratorTestDetail const&) = delete;

  struct Config {
    ToolConfigTable<ReadNextArbitrator::Config> readNextImpl{
      fhicl::Name("readNextImpl")};
  };
  using Parameters = SourceTable<Config>;
  GeneratorTestDetail(Parameters const& ps,
                      art::ProductRegistryHelper& help,
                      art::SourceHelper const& sHelper);

  void closeCurrentFile();

  void readFile(std::string const& name, art::FileBlock*& fb);

  bool readNext(art::RunPrincipal const* const inR,
                art::SubRunPrincipal const* const inSR,
                art::RunPrincipal*& outR,
                art::SubRunPrincipal*& outSR,
                art::EventPrincipal*& outE);

private:
  bool readFileCalled_{false};
  art::SourceHelper const& sHelper_;
  size_t ev_num_{};
  // Test tool invocation from within source.
  std::unique_ptr<art::test::ReadNextArbitrator> readNext_;
};

using namespace art::test;

GeneratorTestDetail::GeneratorTestDetail(Parameters const& p,
                                         art::ProductRegistryHelper&,
                                         art::SourceHelper const& sHelper)
  : sHelper_{sHelper}
  , readNext_{art::make_tool<ReadNextArbitrator>(p().readNextImpl.get_PSet())}
{}

void
GeneratorTestDetail::closeCurrentFile()
{}

void
GeneratorTestDetail::readFile(std::string const& name, art::FileBlock*& fb)
{
  assert(!readFileCalled_);
  assert(name.empty());
  readFileCalled_ = true;
  fb = new art::FileBlock(art::FileFormatVersion(1, "GeneratorTest2012"),
                          "nothing");
}

bool
GeneratorTestDetail::readNext(art::RunPrincipal const* const inR,
                              art::SubRunPrincipal const* const inSR,
                              art::RunPrincipal*& outR,
                              art::SubRunPrincipal*& outSR,
                              art::EventPrincipal*& outE)
{
  art::Timestamp runstart;
  if (++ev_num_ > readNext_->threshold()) {
    return false;
  }
  if (inR == nullptr) {
    outR = sHelper_.makeRunPrincipal(1, runstart);
  }
  if (inSR == nullptr) {
    art::SubRunID srid{outR ? outR->run() : inR->run(), 0};
    outSR = sHelper_.makeSubRunPrincipal(srid.run(), srid.subRun(), runstart);
  }
  outE = sHelper_.makeEventPrincipal(outR ? outR->run() : inR->run(),
                                     outSR ? outSR->subRun() : inSR->subRun(),
                                     ev_num_,
                                     runstart);
  return true;
}

// Trait definition (must precede source typedef).
namespace art {
  template <>
  struct Source_generator<test::GeneratorTestDetail> {
    static constexpr bool value = true;
  };
} // namespace art

// Source declaration.
namespace art::test {
  using GeneratorTest = art::Source<GeneratorTestDetail>;
}

DEFINE_ART_INPUT_SOURCE(art::test::GeneratorTest)
