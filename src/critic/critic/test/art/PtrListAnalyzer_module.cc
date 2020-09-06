#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/test/TestObjects/ToyProducts.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Utilities/ensurePointer.h"
#include "fhiclcpp/ParameterSet.h"

#include <list>
#include <vector>

namespace art::test {

  class PtrListAnalyzer : public art::EDAnalyzer {
  public:
    explicit PtrListAnalyzer(fhicl::ParameterSet const& pset);

  private:
    void analyze(art::Event const& ev) override;
    // input_t is the type of the product we expect to obtain from the
    // Event
    using input_t = std::vector<int>;

    void test_fill_list(art::Event const&);
    void test_fill_vector(art::Event const&);

    std::string const input_label_;
    unsigned const num_expected_;
  };

  PtrListAnalyzer::PtrListAnalyzer(fhicl::ParameterSet const& pset)
    : art::EDAnalyzer{pset}
    , input_label_{pset.get<std::string>("input_label")}
    , num_expected_{pset.get<unsigned>("nvalues")}
  {}

  void
  PtrListAnalyzer::analyze(art::Event const& ev)
  {
    art::Handle<input_t> h;
    ev.getByLabel(input_label_, h);
    assert(h.isValid());
    assert(h->size() == num_expected_);

    test_fill_list(ev);
    test_fill_vector(ev);
  }

  void
  PtrListAnalyzer::test_fill_list(art::Event const& ev)
  {
    // This is how to fill a list of Ptr<T> from a Handle<T>.
    auto const h = ev.getValidHandle<input_t>(input_label_);

    std::list<art::Ptr<int>> ptrs;
    art::fill_ptr_list(ptrs, h);
    assert(ptrs.size() == num_expected_);

    int expected_value = ev.id().event();
    for (auto const& ptr : ptrs) {
      assert(*ptr == expected_value);
      ++expected_value;
    }

    // Basic test of ensurePointer for Ptrs.
    art::ensurePointer<int const*>(ptrs.begin());
    std::list<art::Ptr<int>> const& ptrscref(ptrs);
    art::ensurePointer<int const*>(ptrscref.begin());
  }

  void
  PtrListAnalyzer::test_fill_vector(art::Event const& ev)
  {
    // This is how to fill a vector of Ptr<T> from a Handle<T>.
    auto const h = ev.getValidHandle<input_t>(input_label_);

    std::vector<art::Ptr<int>> ptrs;
    art::fill_ptr_vector(ptrs, h);
    assert(ptrs.size() == num_expected_);

    int expected_value = ev.id().event();
    for (auto const& ptr : ptrs) {
      assert(*ptr == expected_value);
      ++expected_value;
    }
  }

} // namespace art::test

DEFINE_ART_MODULE(art::test::PtrListAnalyzer)
