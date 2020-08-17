////////////////////////////////////////////////////////////////////////
// Class:       GalleryAssnsProducer
// Module Type: producer
// File:        GalleryAssnsProducer_module.cc
//
// This is started as a copy of the AssnsProducer class in critic/test/art.
// Initially the only differences were these name changes:
//   AssnsProducer -> GalleryAssnsProducer
//   arttest -> critictest
//   AssnTestData -> LiteAssnTestData
// And these changes to avoid conflicting with dictionaries in art/test
//   size_t -> int
//   string -> arttest::StringProduct
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/test/TestObjects/ToyProducts.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "critic/test/CriticTestObjects/LiteAssnTestData.h"

#include "cetlib/map_vector.h"

#include <memory>
#include <vector>

using arttest::StringProduct;
using critictest::LiteAssnTestData;
using std::make_unique;
using std::vector;

using art::ProductID;
using art::Ptr;

using intvec = vector<int>;
using stringvec = vector<StringProduct>;
using mapvec = cet::map_vector<StringProduct>;

namespace critictest {
  class GalleryAssnsProducer;
}

class critictest::GalleryAssnsProducer : public art::EDProducer {
public:
  explicit GalleryAssnsProducer(fhicl::ParameterSet const& p);

private:
  void produce(art::Event& e) override;
};

namespace {
  using AssnsAB_t = art::Assns<int, StringProduct, LiteAssnTestData>;
  using AssnsBA_t = art::Assns<StringProduct, int, LiteAssnTestData>;
  using AssnsVoid_t = art::Assns<int, StringProduct>;
} // namespace

critictest::GalleryAssnsProducer::GalleryAssnsProducer(
  fhicl::ParameterSet const& ps)
  : EDProducer{ps}
{
  produces<intvec>();
  produces<stringvec>();
  produces<mapvec>("mv");
  produces<AssnsAB_t>();
  produces<AssnsAB_t>("mapvec");
  produces<AssnsVoid_t>();
  produces<AssnsVoid_t>("mapvec");
  produces<AssnsAB_t>("many");
  produces<AssnsAB_t>("manymapvec");
  produces<AssnsVoid_t>("many");
  produces<AssnsVoid_t>("manymapvec");
}

void
critictest::GalleryAssnsProducer::produce(art::Event& e)
{
  // Create the data products among which we will make associations.
  auto vi = make_unique<intvec>(intvec{2, 0, 1});
  auto vs = make_unique<stringvec>(stringvec{
    StringProduct("one"), StringProduct("two"), StringProduct("zero")});

  // Making a map_vector is hard.
  auto mvs = make_unique<mapvec>();
  using key_t = mapvec::key_type;
  mvs->reserve(3);
  (*mvs)[key_t(0)] = StringProduct{"zero"};
  (*mvs)[key_t(11)] = StringProduct{"one"};
  (*mvs)[key_t(22)] = StringProduct{"two"};

  // We will need the product IDs of the data products.
  auto const vi_pid = e.getProductID<intvec>();
  auto const vs_pid = e.getProductID<stringvec>();
  auto const mvs_pid = e.getProductID<mapvec>("mv");

  // Create the association objects.
  // Assns into vectors.
  auto a = std::make_unique<AssnsAB_t>();
  auto av = std::make_unique<AssnsVoid_t>();

  // Assns into map_vector.
  auto b = std::make_unique<AssnsAB_t>();
  auto bv = std::make_unique<AssnsVoid_t>();

  // addS will add to both x and xv a reference between slot1 of
  // productID1 and slot2 of productID2. The reference in x will have
  // associated data td.
  auto addS = [&e](auto& x,
                   auto& xv,
                   ProductID const id1,
                   std::size_t const slot1,
                   ProductID const id2,
                   std::size_t const slot2,
                   auto const td) {
    x->addSingle(Ptr<int>{id1, slot1, e.productGetter(id1)},
                 Ptr<StringProduct>{id2, slot2, e.productGetter(id2)},
                 td);
    xv->addSingle(Ptr<int>{id1, slot1, e.productGetter(id1)},
                  Ptr<StringProduct>{id2, slot2, e.productGetter(id2)});
  };

  // We add associations in an order such that the associated data are
  // in alphabetical order.
  addS(a, av, vi_pid, 1, vs_pid, 2, LiteAssnTestData{1, 2, "A"});
  addS(b, bv, vi_pid, 1, mvs_pid, 0, LiteAssnTestData{1, 0, "A"});

  addS(a, av, vi_pid, 2, vs_pid, 0, LiteAssnTestData{2, 0, "B"});
  addS(b, bv, vi_pid, 2, mvs_pid, 11, LiteAssnTestData{2, 11, "B"});

  addS(a, av, vi_pid, 0, vs_pid, 1, LiteAssnTestData{0, 1, "C"});
  addS(b, bv, vi_pid, 0, mvs_pid, 22, LiteAssnTestData{0, 22, "C"});

  auto am = make_unique<AssnsAB_t>(*a);
  auto avm = make_unique<AssnsVoid_t>(*av);
  auto bm = make_unique<AssnsAB_t>(*b);
  auto bvm = make_unique<AssnsVoid_t>(*bv);

  addS(am, avm, vi_pid, 1, vs_pid, 2, LiteAssnTestData{1, 2, "AA"});
  addS(bm, bvm, vi_pid, 1, mvs_pid, 0, LiteAssnTestData{1, 0, "AA"});

  e.put(std::move(vi));
  e.put(std::move(vs));
  e.put(std::move(mvs), "mv");
  e.put(std::move(a));
  e.put(std::move(av));
  e.put(std::move(am), "many");
  e.put(std::move(avm), "many");
  e.put(std::move(b), "mapvec");
  e.put(std::move(bv), "mapvec");
  e.put(std::move(bm), "manymapvec");
  e.put(std::move(bvm), "manymapvec");
}

DEFINE_ART_MODULE(critictest::GalleryAssnsProducer)
