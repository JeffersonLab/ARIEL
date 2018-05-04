#ifndef fhiclcpp_make_ParameterSet_h
#define fhiclcpp_make_ParameterSet_h

// ======================================================================
//
// make_ParameterSet - alternate representations -> ParameterSet
//
// ======================================================================

#include "cetlib/filepath_maker.h"
#include "fhiclcpp/fwd.h"

#include <string>

// ----------------------------------------------------------------------

namespace fhicl {

  void make_ParameterSet(intermediate_table const& tbl, ParameterSet& ps);

  void make_ParameterSet(extended_value const& xval, ParameterSet& ps);

  void make_ParameterSet(std::string const& str, ParameterSet& ps);

  void make_ParameterSet(std::string const& filename,
                         cet::filepath_maker& maker,
                         ParameterSet& ps);

} // fhicl

  // ======================================================================

#endif /* fhiclcpp_make_ParameterSet_h */

// Local Variables:
// mode: c++
// End:
