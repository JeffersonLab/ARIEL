//
// Conditions data for a tracker shell.
//

#include "toyExperiment/Conditions/ShellConditions.h"

tex::ShellConditions::ShellConditions()
  : _id(0.), _efficiency(0.), _sigZ(0.), _sigPhi(0.)
{}

tex::ShellConditions::ShellConditions(int id,
                                      double eff,
                                      double sigZ,
                                      double sigPhi)
  : _id(id), _efficiency(eff), _sigZ(sigZ), _sigPhi(sigPhi)
{}
