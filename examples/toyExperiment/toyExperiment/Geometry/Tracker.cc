//
// Describe a cylindrical trackers, made up of shells.
//

#include "toyExperiment/Geometry/Tracker.h"

#include <iostream>

tex::Tracker::Tracker():
  _shells(){
}

void
tex::Tracker::addShell(Shell const& shell){
  _shells.push_back(shell);
}

void
tex::Tracker::print( std::ostream& out ) const{
  for ( std::vector<Shell>::const_iterator i=_shells.begin();
        i != _shells.end();  ++i ){
    out << *i << std::endl;
  }
}

bool
tex::Tracker::insideTracker( CLHEP::Hep3Vector const& p ) const{

  // Assume that the outermost shell encloses all others in both r and z.
  if ( p.perp() > outerRadius() )                      return false;
  if ( std::abs(p.z()) > _shells.back().halfLength() ) return false;
  return true;
}
