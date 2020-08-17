#ifndef Conditions_ParticleInfo_h
#define Conditions_ParticleInfo_h
//
// A minimal particle information class for use with the minimal
// particle data table class.
//

#include "toyExperiment/DataProducts/PDGCode.h"

#include <ostream>
#include <string>

namespace tex {

  class ParticleInfo {

  public:
    ParticleInfo();
    ParticleInfo(PDGCode::type id, double mass, double q, std::string name);

    PDGCode::type
    id() const
    {
      return _id;
    }
    double
    mass() const
    {
      return _mass;
    }
    double
    charge() const
    {
      return _q;
    }
    std::string const&
    name() const
    {
      return _name;
    }

  private:
    PDGCode::type _id;
    double _mass;
    double _q;
    std::string _name;
  };

  std::ostream& operator<<(std::ostream& ost, const ParticleInfo& p);

} // namespace tex

#endif /* Conditions_ParticleInfo_h */
