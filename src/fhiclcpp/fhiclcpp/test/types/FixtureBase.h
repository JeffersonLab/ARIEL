#ifndef fhiclcpp_test_types_FixtureBase_h
#define fhiclcpp_test_types_FixtureBase_h

// ======================================================================
//
// Test struct for BOOST fixture tests
//
// ======================================================================

#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/types/Table.h"

#include <iostream>

namespace fhiclcpp_types {

  template <typename Config>
  struct FixtureBase {

    FixtureBase(std::string const& filename)
      : pset{get_pset(filename)}
      , config{pset.get<fhicl::ParameterSet>("pset"), {}}
    {}

    fhicl::ParameterSet pset;
    fhicl::Table<Config> config;

  private:
    fhicl::ParameterSet
    get_pset(std::string const& filename)
    {
      using namespace fhicl;
      putenv(const_cast<char*>("FHICL_FILE_PATH=./test:."));
      cet::filepath_lookup policy("FHICL_FILE_PATH");
      intermediate_table tbl;
      std::string cfg_in(filename);
      parse_document(cfg_in, policy, tbl);

      ParameterSet pset;
      make_ParameterSet(tbl, pset);
      return pset;
    }
  };
}

#endif /* fhiclcpp_test_types_FixtureBase_h */

// Local variables:
// mode: c++
// End:
