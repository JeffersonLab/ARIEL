//
// Check validity of a parameter set.
//

#include "art-workbook/ExUtilities/PSetChecker.h"

tex::PSetChecker::PSetChecker( std::vector<std::string> const& knownNames,
                                 fhicl::ParameterSet    const& pset,
                                 bool                          allowArtNames ):
  knownNames_(knownNames),
  unknownNames_(){

  // Names of names that are injected by art.
  static std::vector<std::string> artNames = { "module_label", "module_type" };

  for ( auto const& name : pset.get_names() ){

    if ( allowArtNames ) {
      auto j = find( artNames.begin(), artNames.end(), name );
      if ( j != artNames.end() ) continue;
    }

    // All other names must be found in the knownNames list.
    auto i = find( knownNames.begin(), knownNames.end(), name );
    if ( i == knownNames.end() ){
      unknownNames_.emplace_back( name );
    }

  }

}

namespace {

  // A helper function to format a vector<string> into a single, space separated string.
  std::string toString ( std::vector<std::string> const& v ){

    std::string retval;
    for ( auto const& s : v ){
      if ( !retval.empty() ) retval += " ";
      retval += s;
    }
    return retval;

  }

}


std::string tex::PSetChecker::unknownNamesAsString() const{
  return toString( unknownNames_ );
}

std::string tex::PSetChecker::message( std::string const& caller ) const{

  std::string retval = "Unrecognized ParameterSet names found in parameter set for ";
  retval += caller;
  retval += "\nThe unrecognized names are: ";
  retval += unknownNamesAsString();
  retval += "\nThe recognized names are: ";
  retval += toString(knownNames_);

  return retval;

}
