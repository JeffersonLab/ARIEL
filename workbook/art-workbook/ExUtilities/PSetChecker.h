#ifndef ExUtilities_PSetChecker_h
#define ExUtilities_PSetChecker_h

//
// Check validity of a parameter set.
//
// The only check performed is that all of the names are recognized.
// If required parameters are absent, or if values are malformed, this will
// be caught by normal use of the parameter set.
//
// There is no check for range limits.
//
// Fix me: lots of returns can be made move aware.
//

#include "fhiclcpp/ParameterSet.h"

#include <vector>
#include <string>

namespace tex {

  class PSetChecker{

  public:

    PSetChecker( std::vector<std::string> const& knownNames,
                 fhicl::ParameterSet      const& pset,
                 bool                     allowArtNames = true );

    // All names were recognized.
    bool ok()  const { return unknownNames_.empty(); }

    // At least one name was not recognized.
    bool bad() const { return !ok(); }

    // All of the unknown names.
    std::vector<std::string> const& unknownNames() const {
      return unknownNames_;
    }

    // All of the unknown names, formatted as a single, space separated string.
    std::string unknownNamesAsString() const;

    // Format a message appropriate to give to a throw.
    std::string message( std::string const& caller ) const;

  private:

    std::vector<std::string> knownNames_;
    std::vector<std::string> unknownNames_;

  };

} // end namespace tex

#endif /* ExUtilities_PSetChecker_h */
