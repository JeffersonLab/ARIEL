////////////////////////////////////////////////////////////////////////
// Class:       PastimeAssnsProducer
// Plugin Type: producer (art v1_15_01)
// File:        PastimeAssnsProducer_module.cc
//
// Generated at Fri Jul 24 09:36:28 2015 by Chris Green using cetskelgen
// from cetlib version v1_12_02.
////////////////////////////////////////////////////////////////////////

#include "art-workbook/CreatingAssns/PastimeSurveyData.hh"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>

namespace awb {
  class PastimeAssnsProducer;
}


class awb::PastimeAssnsProducer : public art::EDProducer {
public:
  explicit PastimeAssnsProducer(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  PastimeAssnsProducer(PastimeAssnsProducer const &) = delete;
  PastimeAssnsProducer(PastimeAssnsProducer &&) = delete;
  PastimeAssnsProducer & operator = (PastimeAssnsProducer const &) = delete;
  PastimeAssnsProducer & operator = (PastimeAssnsProducer &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  std::string pastimeLabel_;
  std::string peopleLabel_;
  std::string dataLabel_;

};


awb::PastimeAssnsProducer::PastimeAssnsProducer(fhicl::ParameterSet const & p)
 :
  art::EDProducer{p},
  pastimeLabel_(p.get<std::string>("pastimeLabel",
                                   p.get<std::string>("peopleLabel",
                                                      p.get<std::string>("dataLabel")))),
  peopleLabel_(p.get<std::string>("peopleLabel", pastimeLabel_)),
  dataLabel_(p.get<std::string>("dataLabel", pastimeLabel_))
{
  produces<PastimeSurveyData>();
}

void awb::PastimeAssnsProducer::produce(art::Event & e)
{
  auto pastimes [[gnu::unused]] = e.getValidHandle<Pastimes>(pastimeLabel_);
  auto people [[gnu::unused]] = e.getValidHandle<People>(peopleLabel_);
  auto data = e.getValidHandle<PersonalPastimeDataSet>(dataLabel_);
  auto dataIter = data->cbegin();

  //////////////////////////////////////////////////////////////////////
  // Create an empty Assns of the right type and add associations
  // according to the comments following each of the data items in
  // PastimeDataProducer_module.cc. Increment dataIter as we go to
  // provide a check that we haven't missed any. Put the filled Assns
  // into the event.
  //////////////////////////////////////////////////////////////////////

  if (dataIter != data->end()) {
    throw cet::exception("LogicError")
      << "Incorrect number of associations."
      << std::endl;
  }      
}

DEFINE_ART_MODULE(awb::PastimeAssnsProducer)
