#ifndef RecoDataProducts_DetectorStatus_h
#define RecoDataProducts_DetectorStatus_h

//
// A minimal class to mock up detector status information that might be
// produced by the online system.
//
// In this case, the data product is a not a collection type, just to
// show that data products need not be collection types.  It does contain
// a collection type as member data but that is not necessary.
//

#include "toyExperiment/RecoDataProducts/DetectorStatusRecord.h"

#include <vector>
#include <ostream>

namespace tex {

  class DetectorStatus {

  public:

    DetectorStatus( );

    DetectorStatus( size_t n);

    void addRecord( DetectorStatusRecord const& record );

    size_t size() const { return _records.size(); }

    DetectorStatusRecord const& record( size_t id ) const { return _records.at(id); }

    void printAll( std::ostream& ost) const;

  private:

    std::vector<DetectorStatusRecord> _records;

  };

  std::ostream& operator<<(std::ostream& ost,
                           const tex::DetectorStatus& d );

}

#endif /* RecoDataProducts_DetectorStatus_h */
