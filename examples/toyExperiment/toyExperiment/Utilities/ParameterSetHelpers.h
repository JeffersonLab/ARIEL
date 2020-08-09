#ifndef Utilities_ParameterSetHelpers_h
#define Utilities_ParameterSetHelpers_h

// Helper templates for extracting from fhicl::ParameterSet objects of
// types that are not directly supported by fhicl or art.
//
// With this you can write:
//
//    pset.get<CLHEP::Hep3Vector>("position")
//
// in your code.  The fhicl file syntax for specifying vectors is
//
//    position : [ 1.1, 2.2, 3.3 ]
//

#include <string>
#include "fhiclcpp/ParameterSet.h"

namespace CLHEP { class Hep3Vector; }

namespace fhicl {

  // Converting a FHiCL sequence of doubles to a CLHEP::Hep3Vector
  template<> bool ParameterSet::get_if_present<CLHEP::Hep3Vector>(std::string const & key, CLHEP::Hep3Vector& value) const;

}

#endif /*Utilities_ParameterSetHelpers_h */
