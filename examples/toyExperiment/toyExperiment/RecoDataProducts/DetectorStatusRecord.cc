//
// A minimal class to mock up status information about the detector that might be produced
// by the online system.
//

#include "toyExperiment/RecoDataProducts/DetectorStatusRecord.h"

tex::DetectorStatusRecord::DetectorStatusRecord():
  _isOn(false),
  _voltage(0){
}

tex::DetectorStatusRecord::DetectorStatusRecord( bool aisOn, double avoltage ):
  _isOn(aisOn),
  _voltage(avoltage){
}
