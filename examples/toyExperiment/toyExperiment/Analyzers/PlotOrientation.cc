//
// Allowed values of the instance names for Intersection objects,
// as both enum values and strings.
//

#include "toyExperiment/Analyzers/PlotOrientation.h"

std::string const&
tex::PlotOrientationDetail::typeName()
{
  static std::string type("PlotOrientation");
  return type;
}

std::map<tex::PlotOrientationDetail::enum_type, std::string> const&
tex::PlotOrientationDetail::names()
{

  static const std::map<enum_type, std::string> nam = {
    {unknown, "unknown"}, {portrait, "portrait"}, {landscape, "landscape"}};

  return nam;
}
