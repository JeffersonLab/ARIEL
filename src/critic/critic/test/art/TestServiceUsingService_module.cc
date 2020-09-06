#include "cetlib/quiet_unit_test.hpp"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "critic/test/art/ServiceUsing.h"
#include "critic/test/art/Wanted.h"

namespace art::test {
  class TestServiceUsingService;
}

class art::test::TestServiceUsingService : public EDAnalyzer {
public:
  explicit TestServiceUsingService(fhicl::ParameterSet const&);
  ~TestServiceUsingService();

private:
  void analyze(art::Event const&) override;

  void beginJob() override;
  void endJob() override;

  int const debug_level_;
};

art::test::TestServiceUsingService::TestServiceUsingService(
  fhicl::ParameterSet const& p)
  : EDAnalyzer{p}
  , debug_level_{ServiceHandle<ServiceUsing const>{}->getCachedValue()}
{}

art::test::TestServiceUsingService::~TestServiceUsingService()
{
  // Test that art::ServiceHandle can be dereferenced in a module destructor
  ServiceHandle<ServiceUsing const> {}
  ->getCachedValue();
}

void
art::test::TestServiceUsingService::analyze(Event const&)
{
  // NOP.
}

void
art::test::TestServiceUsingService::beginJob()
{
  ServiceHandle<ServiceUsing const> sus;
  BOOST_TEST(debug_level_ == sus->getCachedValue());
  BOOST_TEST(ServiceHandle<Wanted const> {}->getCachedValue() ==
             sus->getCachedValue());
}

void
art::test::TestServiceUsingService::endJob()
{
  ServiceHandle<ServiceUsing const> sus;
  BOOST_TEST(sus->postBeginJobCalled());

  int const current_value{sus->getCachedValue()};
  BOOST_TEST(debug_level_ != current_value);
  BOOST_TEST(ServiceHandle<Wanted const> {}->getCachedValue() == current_value);
}

DEFINE_ART_MODULE(art::test::TestServiceUsingService)
