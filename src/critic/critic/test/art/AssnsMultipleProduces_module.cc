////////////////////////////////////////////////////////////////////////
// Class:       AssnsMultipleProduces
// Module Type: producer
// File:        AssnsMultipleProduces_module.cc
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Persistency/Common/Assns.h"

namespace art::test {
  class AssnsMultipleProduces;
}

class art::test::AssnsMultipleProduces : public EDProducer {
public:
  struct Config {};
  using Parameters = EDProducer::Table<Config>;
  explicit AssnsMultipleProduces(Parameters const&);

private:
  void produce(art::Event&) override{};
};

using product_t = art::Assns<std::size_t, std::string>;
using partner_t = product_t::partner_t;

art::test::AssnsMultipleProduces::AssnsMultipleProduces(Parameters const& ps)
  : EDProducer{ps}
{
  produces<product_t>();
  produces<partner_t>();
}

DEFINE_ART_MODULE(art::test::AssnsMultipleProduces)
