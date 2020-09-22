#ifndef UsePDTService_IdCounter_h
#define UsePDTService_IdCounter_h
//
// Count the number of times each PDG Id appears.
// Print a table of the results when requested.
//

#include "toyExperiment/DataProducts/PDGCode.h"

#include <iosfwd>
#include <map>

namespace tex {

  class IdCounter{

  public:

    typedef std::map<PDGCode::type,int> Counter_type;

    enum outputStyle { full, minimal};

    IdCounter();

    void increment( PDGCode::type id ){
      ++counts_[id];
    }

    Counter_type const& counts() const { return counts_; }

    void print( std::ostream& , outputStyle style=full ) const;

  private:


    Counter_type counts_;

  };

} // end namespace tex

#endif /* UsePDTService_IdCounter_h */
