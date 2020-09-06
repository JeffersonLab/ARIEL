#ifndef Geometry_IntersectionFinder_h
#define Geometry_IntersectionFinder_h

#include "toyExperiment/Geometry/Shell.h"
#include "toyExperiment/RecoDataProducts/Helix.h"

namespace tex {

  class IntersectionFinder {

  public:
    // Check only that the 2D circles intersect.
    IntersectionFinder(double rs, Helix const& helix);

    // Check both that the 2D circles intersect and that the z of intersection
    // is inside the length of the shell.
    IntersectionFinder(Shell const& shell, Helix const& helix);

    double
    arcLength() const
    {
      return s_;
    }
    bool
    hasIntersection() const
    {
      return hasIntersection_;
    }

    CLHEP::Hep3Vector const&
    position() const
    {
      return position_;
    }

  private:
    bool hasIntersection_;

    // 3d arc length along the track, in the forward direction to the first
    // intersection with this shell.
    double s_;

    // Postion of the intersection.
    CLHEP::Hep3Vector position_;
  };

}
#endif /* Geometry_IntersectionFinder_h */
