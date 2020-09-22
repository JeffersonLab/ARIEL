////////////////////////////////////////////////////////////////////////
// Class:       PastimeDataProducer
// Plugin Type: producer (art v1_15_01)
// File:        PastimeDataProducer_module.cc
//
// Generated at Fri Jul 24 09:36:15 2015 by Chris Green using cetskelgen
// from cetlib version v1_12_02.
////////////////////////////////////////////////////////////////////////

#include "art-workbook/CreatingAssns/Pastime.hh"
#include "art-workbook/CreatingAssns/Person.hh"
#include "art-workbook/CreatingAssns/PersonalPastimeData.hh"
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
  class PastimeDataProducer;
}


class awb::PastimeDataProducer : public art::EDProducer {
public:
  explicit PastimeDataProducer(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  PastimeDataProducer(PastimeDataProducer const &) = delete;
  PastimeDataProducer(PastimeDataProducer &&) = delete;
  PastimeDataProducer & operator = (PastimeDataProducer const &) = delete;
  PastimeDataProducer & operator = (PastimeDataProducer &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

};


awb::PastimeDataProducer::PastimeDataProducer(fhicl::ParameterSet const &)
{
  produces<People>();
  produces<Pastimes>();
  produces<PersonalPastimeDataSet>();
}

void awb::PastimeDataProducer::produce(art::Event & e)
{
  auto people =
    std::make_unique<People>
    (People {
      { 770ul, Gender::F, 24u, 16u, 33u }, // 0.
      { 827ul, Gender::M, 42u, 20u, 64u }, // 1.
      { 23ul, Gender::F, 67u, 12u, 30u }, // 2.
      { 11833ul, Gender::M, 33u, 12u, 27u }, // 3.
      { 22009ul, Gender::F, 27u, 12u, 225u } // 4.
    });
  auto pastimes =
    std::make_unique<Pastimes>
    (Pastimes {
      { "Movies", false, false, true, true }, // 0.
      { "Tennis", true, true, true, false }, // 1.
      { "D&D", false, true, true, true }, // 2.
      { "Chess", true, true, true, false }, // 3.
      { "Bottle ships", false, true, true, false }, // 4.
      { "Reading novels", false, false, true, false }, // 5.
      { "Indoor 5-a-side soccer", true, true, true, true }, // 6.
      { "Ultimate frisbee", true, false, false, true }, // 7.
      { "Golf", true, true, false, false } // 8.
    });
  auto ppd =
    std::make_unique<PersonalPastimeDataSet>
    ( PersonalPastimeDataSet
    {
      // (iPerson, iPastime).
      { 20, 25, Proficiency::NA }, // (1, 5).
      { 30, 40, Proficiency::NA }, // (3, 0).
      { 4, 20, Proficiency::JOURNEYMAN }, // (1, 6).
      { 4, 30, Proficiency::NA }, // (0, 0).
      { 5, 30, Proficiency::CASUAL }, // (1, 2).
      { 8, 20, Proficiency::JOURNEYMAN }, // (3, 7).
      { 4, 20, Proficiency::CASUAL }, // (2, 1).
      { 30, 200, Proficiency::EXPERT }, // (2, 8).
      { 4, 0, Proficiency::JOURNEYMAN }, // (2, 3).
      { 10, 50, Proficiency::CASUAL }, // (0, 1).
      { 10, 1000, Proficiency::CASUAL }, // (4, 8).
      { 30, 0, Proficiency::NA }, // (2, 0).
      { 4, 20, Proficiency::CASUAL }, // (4, 7).
      { 4, 0, Proficiency::BEGINNER }, // (4, 2).
      { 20, 20, Proficiency::EXPERT }, // (4, 4).
    });
  e.put(std::move(pastimes));
  e.put(std::move(people));
  e.put(std::move(ppd));
}

DEFINE_ART_MODULE(awb::PastimeDataProducer)
