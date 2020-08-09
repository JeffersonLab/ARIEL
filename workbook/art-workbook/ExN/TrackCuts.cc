//
// A struct to hold a set of cut values.
//

#include "art-workbook/ExN/TrackCuts.h"
#include "art-workbook/ExUtilities/PSetChecker.h"

#include <string>

tex::TrackCuts::TrackCuts( fhicl::ParameterSet const& pset ):
  pmin(pset.get<double>("pmin")),
  minHitsPerTrack(pset.get<int>("minHitsPerTrack"))
{

  std::vector<std::string> knownKeys = { "pmin", "minHitsPerTrack" };
  bool allowArtKeys(false);
  tex::PSetChecker checker( knownKeys, pset, allowArtKeys );
  if ( checker.bad() ){
    throw cet::exception("PSET") << checker.message("TrackCuts") << "\n";
  }

}
