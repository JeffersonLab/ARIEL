#ifndef Conditions_ShellConditions_h
#define Conditions_ShellConditions_h
//
// Conditions data for a tracker shell.
//

#include <ostream>

namespace tex {

  class ShellConditions {

  public:
    ShellConditions();
    ShellConditions(int id, double eff, double sigZ, double sigPhi);

    int
    id() const
    {
      return _id;
    }
    double
    efficiency() const
    {
      return _efficiency;
    }
    double
    sigmaZ() const
    {
      return _sigZ;
    }
    double
    sigmaPhi() const
    {
      return _sigPhi;
    }

  private:
    int _id;
    double _efficiency;
    double _sigZ;
    double _sigPhi;
  };

  inline std::ostream&
  operator<<(std::ostream& ost, const ShellConditions& s)
  {
    ost << "( " << s.id() << ", " << s.efficiency() << ", " << s.sigmaZ()
        << ", " << s.sigmaPhi() << " )";

    return ost;
  }

} // namespace tex

#endif /* Conditions_ShellConditions_h */
