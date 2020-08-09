#ifndef Geometry_Shell_h
#define Geometry_Shell_h

//
// Describe one of the shells that makes up the tracker.
//

#include "toyExperiment/Geometry/TrackerComponent.h"

#include <ostream>

namespace tex {

  class Shell{

  public:

    Shell();

    Shell( int    id,
           TrackerComponent::enum_type section,
           double r,
           double halfLength,
           double halfThickness,
           double sigma );

    int    id()            const { return _id;               }
    double radius()        const { return _radius;           }
    double halfLength()    const { return _halfLength;       }
    double length()        const { return 2.*_halfLength;    }
    double halfThickness() const { return _halfThickness;    }
    double thickness()     const { return 2.*_halfThickness; }
    double sigma()         const { return _sigma;            }

    TrackerComponent trackerComponent() const { return _trackerComponent;  }

  private:

    int              _id;
    TrackerComponent _trackerComponent;
    double           _radius;
    double           _halfLength;
    double           _halfThickness;
    double           _sigma;

  };

  inline std::ostream& operator<<(std::ostream& ost,
                                  const Shell& s ){
    ost << s.id()               << " "
        << s.trackerComponent() << " "
        << s.radius()           << " "
        << s.halfLength()       << " "
        << s.halfThickness()    << " "
        << s.sigma();
    return ost;
  }


}  //namespace mu2e

#endif /* Geometry_Shell_h */
