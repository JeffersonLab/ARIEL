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
  auto refColl = e.getValidHandle<tex::IntersectionCollection>(intersectionTag_);
  art::FindOne<tex::TrkHit> foTrkHits(refColl, e, assnsTag_);
  //////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////
  // Verify that foTrkHits is valid and that it has the same size as the
  // reference collection.
  if (!foTrkHits.isValid()) {
    throw cet::exception("InvalidFindOne")
      << "Unable to obtain valid FindOne object as specified.\n";
  }

  auto const sz = foTrkHits.size();
  auto const sz_ref = refColl->size();
  if (sz != sz_ref) {
    throw cet::exception("BadFindOneSize")
      << "Obtained FindOne has different size ("
      << sz
      << ") to reference collection ("
      << sz_ref
      << ").\n";
  }
  //////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////
  // Count the intersections that have hits and report the total.
  auto count = 0ul;
  for (auto i = 0ul; i != sz; ++i) {
    if (foTrkHits.at(i).isValid()) {
      ++count;
    }
  }

  std::cout << "Found "
            << count
            << " / "
            << sz_ref
            << " intersections with hits in "
            << intersectionTag_
            << std::endl;
  //////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////
  // [Optional]
  //
  // Create a FindMany and verify that it is element-for-element
  // compatible with the FindOne.
  art::FindMany<tex::TrkHit> fmTrkHits(refColl, e, assnsTag_);

  if (!foTrkHits.isValid()) {
    throw cet::exception("InvalidFindMany")
      << "Unable to obtain valid FindMany object as specified.\n";
  }

  auto const sz_fm = fmTrkHits.size();

  if (sz_fm != sz_ref) {
    throw cet::exception("BadFindManySize")
      << "Obtained FindMany has different size ("
      << sz_fm
      << ") to reference collection ("
      << sz_ref
      << ").\n";
  }

  for (auto i = 0ul; i != sz; ++i) {
    auto ass_sz = fmTrkHits.at(i).size();
    auto ass_sz_ref = static_cast<size_t>(foTrkHits.at(i).isValid());
    if (ass_sz != ass_sz_ref) {
      throw cet::exception("ComparisonFailure")
        << "Expected FindMany result of size "
        << ass_sz_ref
        << " at index "
        << i
        << ": got "
        << ass_sz
        << " instead.\n";
    }
  }
  //////////////////////////////////////////////////////////////////////
}

DEFINE_ART_MODULE(awb::AssnsSQO)
