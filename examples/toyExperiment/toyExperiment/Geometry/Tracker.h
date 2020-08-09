#ifndef Geometry_Tracker_h
#define Geometry_Tracker_h
//
// Describe a cylindrical trackers, made up of shells.
//

#include "toyExperiment/Geometry/Shell.h"

#include "CLHEP/Vector/ThreeVector.h"

#include <string>
#include <vector>
#include <ostream>

namespace tex {

  class Tracker{

  public:

    Tracker();

    size_t                    nShells()      const { return _shells.size(); }
    Shell const&              shell( int id ) const { return _shells.at(id); }
    std::vector<Shell> const& shells()        const { return _shells;        }

    double innerRadius() const { return _shells.front().radius(); }
    double outerRadius() const { return _shells.back().radius();  }

    // Is the given point inside the tracker?
    bool insideTracker( CLHEP::Hep3Vector const& position ) const;

    void print( std::ostream& ) const;

    void addShell( Shell const& shell );

    void reserve( size_t n ){
      _shells.reserve(n);
    }

    void clear(){
      _shells.clear();
    }

  private:

    std::vector<Shell> _shells;


  };

  inline std::ostream& operator<<(std::ostream& ost,
                                  const Tracker& t ){
    t.print(ost);
    return ost;
  }

}  // namespace tex

#endif /* Geometry_Tracker_h */
