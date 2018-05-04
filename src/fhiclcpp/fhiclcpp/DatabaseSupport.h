#ifndef fhiclcpp_DatabaseSupport_h
#define fhiclcpp_DatabaseSupport_h

#include <string>
#include <vector>

#include "cetlib/filepath_maker.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "sqlite3.h"

namespace fhicl {
  // Given a ParameterSet, return two vectors of strings:
  //
  //   records: will contain the "database form" of top, and of all
  //            nested ParameterSets.
  //
  //   hashes: will contain the (string form) of the hash for each
  //           ParameterSet in 'records', in the same order.
  void decompose_parameterset(fhicl::ParameterSet const& top,
                              std::vector<std::string>& records,
                              std::vector<std::string>& hashes);

  // Read the file 'filename', creating a ParameterSet, and then call
  // decompose_parameterset using that ParameterSet.
  void decompose_fhicl(std::string const& filename,
                       std::vector<std::string>& records,
                       std::vector<std::string>& hashes);

  // Read the file 'filename', creating a ParameterSet, and then fill
  // the given empty sqlite3 database with all the ParameterSets created
  // by the parsing.
  void parse_file_and_fill_db(std::string const& filename, sqlite3* db);
}

#endif /* fhiclcpp_DatabaseSupport_h */

// Local Variables:
// mode: c++
// End:
