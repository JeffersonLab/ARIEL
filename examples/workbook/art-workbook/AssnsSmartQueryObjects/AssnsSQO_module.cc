////////////////////////////////////////////////////////////////////////
// Class:       AssnsSQO
// Plugin Type: analyzer (art v1_15_01)
// File:        AssnsSQO_module.cc
//
// Generated at Mon Jul 20 14:44:56 2015 by Chris Green using cetskelgen
// from cetlib version v1_12_02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Persistency/Common/FindOne.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "toyExperiment/MCDataProducts/IntersectionCollection.h"
#include "toyExperiment/RecoDataProducts/TrkHitCollection.h"

namespace awb {
  class AssnsSQO;
}


class awb::AssnsSQO : public art::EDAnalyzer {
public:
  explicit AssnsSQO(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  AssnsSQO(AssnsSQO const &) = delete;
  AssnsSQO(AssnsSQO &&) = delete;
  AssnsSQO & operator = (AssnsSQO const &) = delete;
  AssnsSQO & operator = (AssnsSQO &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;

private:

  std::string intersectionTag_;
  std::string assnsTag_;

};


awb::AssnsSQO::AssnsSQO(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p),
  intersectionTag_(p.get<std::string>("intersectionTag")),
  assnsTag_(p.get<std::string>("assnsTag"))
{}

void awb::AssnsSQO::analyze(art::Event const & e [[gnu::unused]])
{
  //////////////////////////////////////////////////////////////////////
  // These two statements should construct correctly the reference
  // collection and a FindOne object using same.
  //
  // auto refColl = e.getValidHandle<tex::IntersectionCollection>(...);
  // art::FindOne<tex::TrkHit> foTrkHits(...);
  //////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////
  // Verify that foTrkHits is valid and that it has the same size as the
  // reference collection.
  //////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////
  // Count the intersections that have hits and report the total.
  //////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////
  // [Optional]
  //
  // Create a FindMany and verify that it is element-for-element
  // compatible with the FindOne.
  //////////////////////////////////////////////////////////////////////
}

DEFINE_ART_MODULE(awb::AssnsSQO)
