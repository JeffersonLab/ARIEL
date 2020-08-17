// ======================================================================
//
// encode overload for extended_value
//
// ======================================================================

#include "cetlib/includer.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/ParameterSetRegistry.h"
#include "fhiclcpp/exception.h"
#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"

using namespace fhicl;

using atom_t = intermediate_table::atom_t;
using complex_t = intermediate_table::complex_t;
using sequence_t = intermediate_table::sequence_t;
using table_t = intermediate_table::table_t;

using ps_atom_t = ParameterSet::ps_atom_t; // same as atom_t!
using ps_sequence_t = ParameterSet::ps_sequence_t;

std::any
fhicl::detail::encode(extended_value const& xval)
{
  switch (xval.tag) {
    case NIL:
    case BOOL:
    case NUMBER:
    case STRING:
      return atom_t(xval);

    case COMPLEX: {
      complex_t const& cmplx = complex_t(xval);
      return '(' + cmplx.first + ',' + cmplx.second + ')';
    }

    case SEQUENCE: {
      ps_sequence_t result;
      sequence_t const& seq = sequence_t(xval);
      for (sequence_t::const_iterator it = seq.begin(), e = seq.end(); it != e;
           ++it)
        result.push_back(std::any(encode(*it)));
      return result;
    }

    case TABLE: {
      typedef table_t::const_iterator const_iterator;
      table_t const& tbl = table_t(xval);
      ParameterSet result;
      for (const_iterator it = tbl.begin(), e = tbl.end(); it != e; ++it) {
        if (!it->second.in_prolog)
          result.put(it->first, it->second);
      }
      return ParameterSetRegistry::put(result);
    }

    case TABLEID: {
      atom_t const& tableID = atom_t(xval);
      return ParameterSetID(tableID);
    }

    case UNKNOWN:
    default: {
      throw fhicl::exception(type_mismatch, "unknown extended value");
    }
  }
} // encode()
