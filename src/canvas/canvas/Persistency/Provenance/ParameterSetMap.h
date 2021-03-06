#ifndef canvas_Persistency_Provenance_ParameterSetMap_h
#define canvas_Persistency_Provenance_ParameterSetMap_h
#include "canvas/Persistency/Provenance/ParameterSetBlob.h"
#include "fhiclcpp/ParameterSetID.h"
#include <map>

namespace art {
  typedef std::map<fhicl::ParameterSetID, ParameterSetBlob> ParameterSetMap;
}

#endif /* canvas_Persistency_Provenance_ParameterSetMap_h */

// Local Variables:
// mode: c++
// End:
