#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/test/TestObjects/ToyProducts.h"
#include "fhiclcpp/ParameterSet.h"

#include <memory>
#include <string>
#include <vector>

using arttest::IntProduct;
using arttest::SimpleDerived;
using arttest::StringProduct;

namespace critictest {

  class GalleryTestProducer : public art::EDProducer {
  public:
    explicit GalleryTestProducer(fhicl::ParameterSet const& p);

  private:
    void produce(art::Event& e) override;

    int const value1_;
    int const value2_;
    int const value3_;

    std::string const string1_;
    std::string const string2_;
    std::string const string3_;
  };

  GalleryTestProducer::GalleryTestProducer(fhicl::ParameterSet const& pset)
    : EDProducer{pset}
    , value1_(pset.get<int>("value1"))
    , value2_(pset.get<int>("value2"))
    , value3_(pset.get<int>("value3"))
    , string1_(pset.get<std::string>("string1"))
    , string2_(pset.get<std::string>("string2"))
    , string3_(pset.get<std::string>("string3"))
  {
    produces<IntProduct>();
    produces<IntProduct>("i2");
    produces<IntProduct>("i3");

    produces<StringProduct>();
    produces<StringProduct>("i2");
    produces<StringProduct>("i3");

    produces<IntProduct>("eventID");

    produces<std::vector<int>>();
    produces<std::vector<SimpleDerived>>("SimpleDerived");
    produces<std::vector<int>>("willBeDropped");
    produces<std::vector<StringProduct>>();
  }

  void
  GalleryTestProducer::produce(art::Event& event)
  {
    if (value1_ > 0) {
      event.put(std::unique_ptr<IntProduct>(new IntProduct(value1_)));
    }
    if (value2_ > 0) {
      event.put(std::unique_ptr<IntProduct>(new IntProduct(value2_)), "i2");
    }
    if (value3_ > 0) {
      event.put(std::unique_ptr<IntProduct>(new IntProduct(value3_)), "i3");
    }

    if (string1_ != "noPut") {
      event.put(std::unique_ptr<StringProduct>(new StringProduct(string1_)));
    }
    if (string2_ != "noPut") {
      event.put(std::unique_ptr<StringProduct>(new StringProduct(string2_)),
                "i2");
    }
    if (string3_ != "noPut") {
      event.put(std::unique_ptr<StringProduct>(new StringProduct(string3_)),
                "i3");
    }
    event.put(std::unique_ptr<IntProduct>(new IntProduct(event.event())),
              "eventID");

    std::unique_ptr<std::vector<int>> vecInt(new std::vector<int>);
    vecInt->push_back(value1_ + event.event());
    vecInt->push_back(value2_);
    vecInt->push_back(value3_);
    event.put(std::move(vecInt));

    std::unique_ptr<std::vector<SimpleDerived>> vecSimpleDerived(
      new std::vector<SimpleDerived>);
    SimpleDerived sd1;
    sd1.key = value1_;
    sd1.value = 0.0;
    vecSimpleDerived->push_back(sd1);
    SimpleDerived sd2;
    sd2.key = value2_;
    sd2.value = 0.0;
    vecSimpleDerived->push_back(sd2);
    SimpleDerived sd3;
    sd3.key = value3_;
    sd3.value = 0.0;
    vecSimpleDerived->push_back(sd3);
    event.put(std::move(vecSimpleDerived), "SimpleDerived");

    std::unique_ptr<std::vector<int>> vecIntD(new std::vector<int>);
    vecIntD->push_back(value1_ + event.event());
    vecIntD->push_back(value2_);
    vecIntD->push_back(value3_);
    event.put(std::move(vecIntD), "willBeDropped");

    std::unique_ptr<std::vector<StringProduct>> vecStringProduct(
      new std::vector<StringProduct>);
    vecStringProduct->push_back(StringProduct(string1_));
    vecStringProduct->push_back(StringProduct(string2_));
    vecStringProduct->push_back(StringProduct(string3_));
    event.put(std::move(vecStringProduct));
  }
} // namespace critictest

DEFINE_ART_MODULE(critictest::GalleryTestProducer)
