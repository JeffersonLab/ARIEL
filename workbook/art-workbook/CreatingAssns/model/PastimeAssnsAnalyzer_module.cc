////////////////////////////////////////////////////////////////////////
// Class:       PastimeAssnsAnalyzer
// Plugin Type: analyzer (art v1_15_01)
// File:        PastimeAssnsAnalyzer_module.cc
//
// Generated at Fri Jul 24 09:37:24 2015 by Chris Green using cetskelgen
// from cetlib version v1_12_02.
////////////////////////////////////////////////////////////////////////


#include "art-workbook/CreatingAssns/PastimeSurveyData.hh"
#include "art/Framework/Core/EDAnalyzer.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <iostream>

namespace awb {
  class PastimeAssnsAnalyzer;
}


class awb::PastimeAssnsAnalyzer : public art::EDAnalyzer {
public:
  explicit PastimeAssnsAnalyzer(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  PastimeAssnsAnalyzer(PastimeAssnsAnalyzer const &) = delete;
  PastimeAssnsAnalyzer(PastimeAssnsAnalyzer &&) = delete;
  PastimeAssnsAnalyzer & operator = (PastimeAssnsAnalyzer const &) = delete;
  PastimeAssnsAnalyzer & operator = (PastimeAssnsAnalyzer &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;

private:

  std::string surveyLabel_;
  std::string peopleLabel_;

};


awb::PastimeAssnsAnalyzer::PastimeAssnsAnalyzer(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p),
  surveyLabel_(p.get<std::string>("surveyLabel")),
  peopleLabel_(p.get<std::string>("peopleLabel"))
{}

void awb::PastimeAssnsAnalyzer::analyze(art::Event const & e)
{
  auto survey = e.getValidHandle<PastimeSurveyData>(surveyLabel_);

  auto costThreshold = 20u;
  std::size_t expensiveCount { 0ul };

  //////////////////////////////////////////////////////////////////////
  // Calculate expensiveCount from the contents of survey.
  auto const end = survey->end();
  for (auto it = survey->begin(); it != end; ++it) {
    if (survey->data(it).cost > costThreshold) {
      ++expensiveCount;
    }
  }
  //////////////////////////////////////////////////////////////////////

  std::cout
    << "Counted "
    << expensiveCount
    << " expensive (>$"
    << costThreshold
    << "/mo) pastimes among the survey population."
    << std::endl;

  std::size_t const nPastimesThreshold = 3ul;
  std::size_t nHedonists { 0ul };

  //////////////////////////////////////////////////////////////////////
  // Make a suitable FindMany and use it to calculate nHedonists.
  art::FindMany<Pastime, PersonalPastimeData>
    pastimes(e.getValidHandle<People>(peopleLabel_), e, surveyLabel_);

  auto const nPeople = pastimes.size();
  for (std::size_t i = 0ul; i != nPeople; ++i) {
    if (pastimes.at(i).size() > nPastimesThreshold) {
      ++nHedonists;
    }
  }
  //////////////////////////////////////////////////////////////////////

  std::cout
    << "Found "
    << nHedonists
    << " people with more than "
    << nPastimesThreshold
    << " pastimes."
    << std::endl;
}

DEFINE_ART_MODULE(awb::PastimeAssnsAnalyzer)
