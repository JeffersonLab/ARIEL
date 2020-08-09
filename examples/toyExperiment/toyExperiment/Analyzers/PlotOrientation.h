#ifndef Analyzers_PlotOrientation_h
#define Analyzers_PlotOrientation_h
//
// Allowed values of the plot orientation of the event display
//

#include "toyExperiment/DataProducts/EnumToStringSparse.h"

namespace tex {

  class PlotOrientationDetail{
  public:

    enum enum_type { unknown=-1, portrait, landscape };

    static std::string const& typeName();

    static std::map<enum_type,std::string> const& names();

  };

  typedef EnumToStringSparse<PlotOrientationDetail> PlotOrientation;
}

#endif
