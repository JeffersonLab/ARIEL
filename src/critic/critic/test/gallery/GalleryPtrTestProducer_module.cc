#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/test/TestObjects/ToyProducts.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Utilities/InputTag.h"
#include "critic/test/CriticTestObjects/LiteAssnTestData.h"
#include "critic/test/CriticTestObjects/LitePtrTestProduct.h"

#include <memory>
#include <vector>

namespace fhicl {
  class ParameterSet;
}

using arttest::Simple;
using arttest::SimpleDerived;
using arttest::StringProduct;

namespace critictest {

  class GalleryPtrTestProducer : public art::EDProducer {
  public:
    using AssnsAB_t = art::Assns<StringProduct, int, LiteAssnTestData>;
    using AssnsBA_t = art::Assns<int, StringProduct, LiteAssnTestData>;

    explicit GalleryPtrTestProducer(fhicl::ParameterSet const&);

  private:
    void produce(art::Event&) override;

    bool const produceAssnStringInt_;
    bool const produceAssnIntString_;
    art::ProductToken<std::vector<int>> const int1Token_;
    art::ProductToken<std::vector<int>> const int2Token_;
    art::ProductToken<std::vector<int>> const int3Token_;
    art::ProductToken<std::vector<SimpleDerived>> const simpleDerived1Token_;
    art::ProductToken<std::vector<SimpleDerived>> const simpleDerived2Token_;
    art::ProductToken<std::vector<SimpleDerived>> const simpleDerived3Token_;
    art::ProductToken<std::vector<StringProduct>> const stringProductToken_;
    art::ProductToken<std::vector<int>> const dropToken_;
  };

  GalleryPtrTestProducer::GalleryPtrTestProducer(
    fhicl::ParameterSet const& pset)
    : EDProducer{pset}
    , produceAssnStringInt_(pset.get<bool>("produceAssnStringInt", false))
    , produceAssnIntString_(pset.get<bool>("produceAssnIntString", false))
    , int1Token_{consumes<std::vector<int>>("m1::PROD1")}
    , int2Token_{consumes<std::vector<int>>("m1::PROD2")}
    , int3Token_{consumes<std::vector<int>>("m1::PROD3")}
    , simpleDerived1Token_{consumes<std::vector<SimpleDerived>>(
        "m1:SimpleDerived:PROD1")}
    , simpleDerived2Token_{consumes<std::vector<SimpleDerived>>(
        "m1:SimpleDerived:PROD2")}
    , simpleDerived3Token_{consumes<std::vector<SimpleDerived>>(
        "m1:SimpleDerived:PROD3")}
    , stringProductToken_{consumes<std::vector<StringProduct>>("m1::PROD1")}
    , dropToken_{consumes<std::vector<int>>("m1:willBeDropped:PROD1")}
  {
    produces<LitePtrTestProduct>();

    if (produceAssnStringInt_) {
      produces<AssnsAB_t>();
    }

    if (produceAssnIntString_) {
      produces<AssnsBA_t>();
    }
  }

  void
  GalleryPtrTestProducer::produce(art::Event& event)
  {
    auto testProduct = std::make_unique<LitePtrTestProduct>();

    auto const& hint1 = event.getValidHandle(int1Token_);
    auto const& hint2 = event.getValidHandle(int2Token_);
    auto const& hint3 = event.getValidHandle(int3Token_);
    testProduct->ptrInt1 = art::Ptr<int>{hint1, 0};
    testProduct->ptrInt2 = art::Ptr<int>{hint2, 1};
    testProduct->ptrInt3 = art::Ptr<int>{hint3, 2};

    auto const& hSimpleDerived1 = event.getValidHandle(simpleDerived1Token_);
    auto const& hSimpleDerived2 = event.getValidHandle(simpleDerived2Token_);
    auto const& hSimpleDerived3 = event.getValidHandle(simpleDerived3Token_);
    testProduct->ptrSimple1 = art::Ptr<Simple>{hSimpleDerived1, 0};
    testProduct->ptrSimple2 = art::Ptr<Simple>{hSimpleDerived2, 1};
    testProduct->ptrSimple3 = art::Ptr<Simple>{hSimpleDerived3, 2};

    testProduct->ptrSimpleDerived1 =
      art::Ptr<SimpleDerived>{hSimpleDerived1, 0};
    testProduct->ptrSimpleDerived2 =
      art::Ptr<SimpleDerived>{hSimpleDerived2, 1};
    testProduct->ptrSimpleDerived3 =
      art::Ptr<SimpleDerived>{hSimpleDerived3, 2};

    testProduct->ptrVectorInt.push_back(art::Ptr<int>{hint1, 0});
    testProduct->ptrVectorInt.push_back(art::Ptr<int>{hint1, 2});

    testProduct->ptrVectorSimple.push_back(
      art::Ptr<Simple>{hSimpleDerived1, 0});
    testProduct->ptrVectorSimple.push_back(
      art::Ptr<Simple>{hSimpleDerived1, 2});

    testProduct->ptrVectorSimpleDerived.push_back(
      art::Ptr<SimpleDerived>{hSimpleDerived1, 0});
    testProduct->ptrVectorSimpleDerived.push_back(
      art::Ptr<SimpleDerived>{hSimpleDerived1, 2});

    auto const& hDrop = event.getValidHandle(dropToken_);
    testProduct->ptrIntoContainerToBeDropped = art::Ptr<int>{hDrop, 1};
    testProduct->nullPtr = art::Ptr<int>{hint1.id()};
    testProduct->nullDroppedPtr = art::Ptr<int>{hDrop.id()};
    testProduct->invalidPtr = art::Ptr<int>{};

    event.put(move(testProduct));

    auto const& hVStringProduct = event.getValidHandle(stringProductToken_);
    if (produceAssnStringInt_) {
      auto assnsAB = std::make_unique<AssnsAB_t>();
      assnsAB->addSingle(art::Ptr<StringProduct>{hVStringProduct, 0},
                         art::Ptr<int>{hint1, 1},
                         LiteAssnTestData{0, 1, "A"});
      assnsAB->addSingle(art::Ptr<StringProduct>{hVStringProduct, 1},
                         art::Ptr<int>{hint1, 2},
                         LiteAssnTestData{1, 2, "B"});
      event.put(move(assnsAB));
    }

    if (produceAssnIntString_) {
      auto assnsBA = std::make_unique<AssnsBA_t>();
      assnsBA->addSingle(art::Ptr<int>{hint1, 2},
                         art::Ptr<StringProduct>{hVStringProduct, 2},
                         LiteAssnTestData{1, 2, "C"});
      assnsBA->addSingle(art::Ptr<int>{hint1, 1},
                         art::Ptr<StringProduct>{hVStringProduct, 0},
                         LiteAssnTestData{1, 2, "D"});
      event.put(move(assnsBA));
    }
  }
} // namespace critictest

DEFINE_ART_MODULE(critictest::GalleryPtrTestProducer)
