//--------------------------------------------------------------------
// Produces an IntProduct instance.
//--------------------------------------------------------------------

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/SharedProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Persistency/Provenance/BranchType.h"
#include "critic/test/art/RunTimeProduces.h"

#include "fhiclcpp/types/Atom.h"

#include "art/test/TestObjects/ToyProducts.h"

#include <iostream>
#include <memory>

using namespace fhicl;

namespace arttest {
  class IntProducer;
}

using arttest::IntProducer;

class arttest::IntProducer : public art::SharedProducer {
public:
  struct Config {
    Atom<int> ivalue{Name("ivalue")};
    Atom<unsigned long> branchType{
      Name("branchType"),
      Comment{"The 'branchType' argument specifies the level for which\n"
              "the product is made.  Available options include:\n"
              "  0 (event)\n"
              "  1 (subrun)\n"
              "  2 (run)\n"},
      art::InEvent};
  };
  using Parameters = Table<Config>;
  explicit IntProducer(Parameters const& p, art::ProcessingFrame const&);

private:
  void produce(art::Event& e, art::ProcessingFrame const&) override;
  void endSubRun(art::SubRun& sr, art::ProcessingFrame const&) override;
  void endRun(art::Run& r, art::ProcessingFrame const&) override;

  int const value_;
  art::BranchType const branchType_;
}; // IntProducer

IntProducer::IntProducer(Parameters const& p, art::ProcessingFrame const&)
  : art::SharedProducer{p}
  , value_{p().ivalue()} // enums don't usually have a conversion from string
  , branchType_{art::BranchType(p().branchType())}
{
  art::test::run_time_produces<IntProduct>(producesCollector(), branchType_);
  async<art::InEvent>();
}

void
IntProducer::produce(art::Event& e, art::ProcessingFrame const&)
{
  if (branchType_ == art::InEvent)
    e.put(std::make_unique<IntProduct>(value_));
}

void
IntProducer::endSubRun(art::SubRun& sr, art::ProcessingFrame const&)
{
  if (branchType_ == art::InSubRun)
    sr.put(std::make_unique<IntProduct>(value_), art::subRunFragment());
}

void
IntProducer::endRun(art::Run& r, art::ProcessingFrame const&)
{
  if (branchType_ == art::InRun)
    r.put(std::make_unique<IntProduct>(value_), art::runFragment());
}

DEFINE_ART_MODULE(IntProducer)
