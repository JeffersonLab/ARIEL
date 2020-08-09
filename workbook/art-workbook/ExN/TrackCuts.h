#ifndef ExN_TrackCuts_h
#define ExN_TrackCuts_h

//
// A struct to hold a set of cut values.
//

#include "fhiclcpp/ParameterSet.h"

#include <string>

namespace tex {

  struct TrackCuts {

    TrackCuts():pmin(0),minHitsPerTrack(0){}

    TrackCuts( double apmin, int aminHits ):pmin(apmin),minHitsPerTrack(aminHits){}

    TrackCuts( fhicl::ParameterSet const& pset );

    double pmin;
    int    minHitsPerTrack;

  };


} // end namespace tex

#endif /* ExN_TrackTrackCuts_h */
