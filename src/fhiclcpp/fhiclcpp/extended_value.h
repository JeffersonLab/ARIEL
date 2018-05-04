#ifndef fhiclcpp_extended_value_h
#define fhiclcpp_extended_value_h

// ======================================================================
//
// extended_value
//
// ======================================================================

#include "boost/any.hpp"
#include "fhiclcpp/Protection.h"
#include "fhiclcpp/fwd.h"
#include "stdmap_shims.h"

#include <map>
#include <string>
#include <vector>

// ----------------------------------------------------------------------

namespace fhicl {
  enum value_tag {
    UNKNOWN,
    NIL,
    BOOL,
    NUMBER,
    COMPLEX,
    STRING,
    SEQUENCE,
    TABLE,
    TABLEID
  };
}

// ----------------------------------------------------------------------

class fhicl::extended_value {
public:
  using atom_t = std::string;
  using complex_t = std::pair<std::string, std::string>;
  using sequence_t = std::vector<extended_value>;
  using table_t = shims::map<std::string, extended_value>;

  extended_value() = default;

  extended_value(bool const in_prolog,
                 value_tag const tag,
                 boost::any const value,
                 Protection const protection,
                 std::string const& src = {})
    : in_prolog{in_prolog}
    , tag{tag}
    , value{value}
    , src_info{src}
    , protection{protection}
  {}

  extended_value(bool const in_prolog,
                 value_tag const tag,
                 boost::any const value,
                 std::string const& src = {})
    : in_prolog{in_prolog}, tag{tag}, value{value}, src_info{src}
  {}

  bool
  is_a(value_tag t) const
  {
    return t == tag;
  }

  std::string to_string() const;

  void set_prolog(bool new_prolog_state);

  void
  set_src_info(std::string const& src)
  {
    src_info = src;
  }

  std::string pretty_src_info() const;

  operator atom_t() const { return boost::any_cast<atom_t>(value); }

  operator complex_t() const { return boost::any_cast<complex_t>(value); }

  operator sequence_t() const { return boost::any_cast<sequence_t>(value); }

  operator table_t() const { return boost::any_cast<table_t>(value); }

  bool in_prolog{false};
  value_tag tag{UNKNOWN};
  boost::any value{};
  std::string src_info{};
  Protection protection{Protection::NONE};

}; // extended_value

// ======================================================================

#endif /* fhiclcpp_extended_value_h */

// Local Variables:
// mode: c++
// End:
