#ifndef Utilities_ParameterSetFromFile_h
#define Utilities_ParameterSetFromFile_h

//
// Read a .fcl file and form a parameter set object.
//

#include "fhiclcpp/ParameterSet.h"

#include <iosfwd>
#include <string>

namespace tex {

  class ParameterSetFromFile {

  public:
    ParameterSetFromFile(std::string const& fileName);

    fhicl::ParameterSet const&
    pSet() const
    {
      return _pSet;
    }

    void printNames(std::ostream&) const;

  private:
    std::string _fileName;
    fhicl::ParameterSet _pSet;
  };

}

#endif /* Utilities_ParameterSetFromFile_h */
