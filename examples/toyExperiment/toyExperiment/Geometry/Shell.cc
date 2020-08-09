//
// Describe one of the shells that makes up the tracker.
//

#include "toyExperiment/Geometry/Shell.h"

tex::Shell::Shell(int    id,
                  TrackerComponent::enum_type trackerComponent,
                  double radius,
                  double halfLength,
                  double halfThickness,
                  double sigma ):
  _id(id),
  _trackerComponent(trackerComponent),
  _radius(radius),
  _halfLength(halfLength),
  _halfThickness(halfThickness),
  _sigma(sigma){
}

tex::Shell::Shell():
  _id(-1),
  _trackerComponent(),
  _radius(0),
  _halfLength(0),
  _halfThickness(0),
  _sigma(0){
}
