// ======================================================================
//
// test of to_indented_string with annotations
//
// ======================================================================

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include <cstdlib>
#include <iostream>
#include <string>

int
main()
{

  // Configuration file lookup policy.
  char const* fhicl_env = getenv("FHICL_FILE_PATH");
  std::string search_path;
  if (fhicl_env == nullptr) {
    std::cerr << "Expected environment variable FHICL_FILE_PATH is "
              << "missing or empty: using \".\"\n";
    search_path = ".:";
  } else {
    search_path = std::string(fhicl_env) + ":";
  }

  cet::filepath_lookup policy(search_path);
  std::string const in{"to_indented_string_annotated_test.fcl"};

  fhicl::intermediate_table tbl1;
  fhicl::parse_document(in, policy, tbl1);
  fhicl::ParameterSet pset;
  fhicl::make_ParameterSet(tbl1, pset);

  pset.put<int>("another_number", 4);
  pset.put_or_replace<int>("some_number", 8);
  pset.put_or_replace<double>("double", 234.6);

  std::cout << pset.to_indented_string(0, true) << std::endl;
}
