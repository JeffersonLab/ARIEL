#ifndef Geometry_SupportModel_h
#define Geometry_SupportModel_h
//
// Allowed values of the instance names for Intersection objects,
// as both enum values and strings.
//
// Notes
// 1) inner and outer will be used as indices into std::vectors, so they must be
// 0 based.
//    Therefore we need to use the EnumToStringSparse, not EnumToString, which
//    requires that the unknown enum value be 0.
//

#include "toyExperiment/DataProducts/EnumToStringSparse.h"

namespace tex {

  class TrackerComponentDetail {
  public:
    // inner and outer will be used as indices into std::vectors.  See note 1.
    enum enum_type { unknown = -1, inner, outer };

    static std::string const& typeName();

    static std::map<enum_type, std::string> const& names();
  };

  typedef EnumToStringSparse<TrackerComponentDetail> TrackerComponent;
}

#endif
