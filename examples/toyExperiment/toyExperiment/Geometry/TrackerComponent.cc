//
// Allowed values of the instance names for Intersection objects,
// as both enum values and strings.
//

#include "toyExperiment/Geometry/TrackerComponent.h"

std::string const&
tex::TrackerComponentDetail::typeName()
{
  static std::string type("TrackerComponent");
  return type;
}

std::map<tex::TrackerComponentDetail::enum_type, std::string> const&
tex::TrackerComponentDetail::names()
{

  static std::map<enum_type, std::string> nam;

  if (nam.empty()) {
    nam[unknown] = "unknown";
    nam[inner] = "inner";
    nam[outer] = "outer";
  }

  return nam;
}
