// ======================================================================
//
// extended_value
//
// ======================================================================

#include "fhiclcpp/extended_value.h"

#include <regex>

using std::any_cast;
using std::string;

// ----------------------------------------------------------------------

std::string
fhicl::extended_value::to_string() const
{
  if (in_prolog && !shims::isSnippetMode())
    return "";

  switch (tag) {

    case NIL:
    case BOOL:
    case NUMBER:
    case STRING: {
      return any_cast<atom_t>(value);
    }

    case COMPLEX: {
      auto c = any_cast<complex_t>(value);
      return '(' + c.first + ',' + c.second + ')';
    }

    case SEQUENCE: {
      auto q = any_cast<sequence_t>(value);
      string s("[");
      string sep;
      for (auto const& v : q) {
        s.append(sep).append(v.to_string());
        sep = ",";
      }
      return s + ']';
    }

    case TABLE: {
      auto t = any_cast<table_t>(value);
      string s("{");
      string sep;
      for (auto const& pr : t) {
        s.append(sep).append(pr.first + ':' + pr.second.to_string());
        sep = " ";
      }
      return s + '}';
    }

    case TABLEID: {
      return string("@id::") + any_cast<atom_t>(value);
    }

    case UNKNOWN:
    default: {
      return "";
    }

  }; // switch

} // to_string()

// ----------------------------------------------------------------------

void
fhicl::extended_value::set_prolog(bool new_prolog_state)
{
  in_prolog = new_prolog_state;

  switch (tag) {

    case NIL:
    case BOOL:
    case NUMBER:
    case STRING:
    case COMPLEX:
    case TABLEID: {
      break;
    }

    case SEQUENCE: {
      auto& q = any_cast<sequence_t&>(value);
      for (auto& e : q) {
        e.set_prolog(new_prolog_state);
      }
      break;
    }

    case TABLE: {
      auto& t = any_cast<table_t&>(value);
      for (auto& pr : t) {
        pr.second.set_prolog(new_prolog_state);
      }
      break;
    }

    case UNKNOWN:
    default: {
      break;
    }

  }; // switch

} // set_prolog()

std::string
fhicl::extended_value::pretty_src_info() const
{
  std::string result;
  static std::regex const splitRE("(.*):([0-9-]*)");
  std::smatch m;
  if (std::regex_match(src_info, m, splitRE)) {
    result =
      std::string("line ") + m[2].str() + " of file \"" + m[1].str() + '"';
  } else {
    result = "<unknown>";
  }
  return result;
}

// ======================================================================
