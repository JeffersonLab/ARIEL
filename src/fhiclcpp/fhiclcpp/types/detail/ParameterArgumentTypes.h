#ifndef fhiclcpp_types_detail_ParameterArgumentTypes_h
#define fhiclcpp_types_detail_ParameterArgumentTypes_h

#include <string>

namespace fhicl {

  enum class par_type {
    ATOM,
    TABLE,
    SEQ_VECTOR,
    SEQ_ARRAY,
    TUPLE,
    DELEGATE,
    NTYPES
  };

  inline bool
  is_table(par_type const pt)
  {
    return pt == par_type::TABLE;
  }

  inline bool
  is_sequence(par_type const pt)
  {
    return pt == par_type::SEQ_VECTOR || pt == par_type::SEQ_ARRAY ||
           pt == par_type::TUPLE;
  }

  inline bool
  is_atom(par_type const pt)
  {
    return pt == par_type::ATOM;
  }

  enum class par_style {
    REQUIRED,
    REQUIRED_CONDITIONAL,
    DEFAULT,
    DEFAULT_CONDITIONAL,
    OPTIONAL,
    OPTIONAL_CONDITIONAL,
    NTYPES
  };
}

#endif /* fhiclcpp_types_detail_ParameterArgumentTypes_h */

// Local variables:
// mode: c++
// End:
