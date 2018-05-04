// ======================================================================
//
// make_ParameterSet - intermediate_table -> ParameterSet
//
// ======================================================================

#include "fhiclcpp/make_ParameterSet.h"

#include "boost/any.hpp"
#include "cetlib/includer.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/ParameterSetRegistry.h"
#include "fhiclcpp/exception.h"
#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"

using namespace fhicl;

using atom_t = intermediate_table::atom_t;
using complex_t = intermediate_table::complex_t;
using sequence_t = intermediate_table::sequence_t;
using table_t = intermediate_table::table_t;

using ps_atom_t = ParameterSet::ps_atom_t; // same as atom_t!
using ps_sequence_t = ParameterSet::ps_sequence_t;

// ----------------------------------------------------------------------

void
fhicl::make_ParameterSet(intermediate_table const& tbl, ParameterSet& ps)
{
  for (auto const& pr : tbl) {
    if (!pr.second.in_prolog)
      ps.put(pr.first, pr.second);
  }
}

// ----------------------------------------------------------------------

void
fhicl::make_ParameterSet(extended_value const& xval, ParameterSet& ps)
{
  if (!xval.is_a(TABLE))
    throw fhicl::exception(type_mismatch, "extended value not a table");

  table_t const& tbl = table_t(xval);
  for (auto const& pr : tbl) {
    if (!pr.second.in_prolog)
      ps.put(pr.first, pr.second);
  }
}

// ----------------------------------------------------------------------

void
fhicl::make_ParameterSet(std::string const& str, ParameterSet& ps)
{
  intermediate_table tbl;
  parse_document(str, tbl);
  make_ParameterSet(tbl, ps);
}

// ----------------------------------------------------------------------

void
fhicl::make_ParameterSet(std::string const& filename,
                         cet::filepath_maker& maker,
                         ParameterSet& ps)
{
  intermediate_table tbl;
  parse_document(filename, maker, tbl);
  make_ParameterSet(tbl, ps);
}

// ======================================================================
