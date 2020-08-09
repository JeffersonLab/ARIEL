#ifndef RecoDataProducts_DetectorStatusRecord_h
#define RecoDataProducts_DetectorStatusRecord_h

//
// A minimal class to mock up status information that might be produced by the online system.
// It represents status information about one of the detector elements.
//

#include <ostream>

namespace tex {

  class DetectorStatusRecord {

  public:

    DetectorStatusRecord();

    DetectorStatusRecord( bool isOn, double voltage );

    bool   isOn   ( ) const { return  _isOn;    }
    bool   isOff  ( ) const { return !_isOn;    }
    double voltage( ) const { return  _voltage; }

  private:

    bool   _isOn;
    double _voltage;

  };

  inline std::ostream& operator<<(std::ostream& ost,
                                  const tex::DetectorStatusRecord& d ){
    ost << d.isOn() << " " << d.voltage();
    return ost;
  }

}

#endif /* RecoDataProducts_DetectorStatusRecord_h */
