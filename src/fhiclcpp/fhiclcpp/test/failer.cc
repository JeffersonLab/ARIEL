#include "cetlib_except/exception.h"
#include "cetlib/filepath_maker.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"
#include <cstdlib>
#include <iostream>

int
main()
{
  putenv(const_cast<char*>("FHICL_FILE_PATH=./test:."));

  std::string in("test_config_fail.fcl");
  cet::filepath_lookup policy("FHICL_FILE_PATH");
  fhicl::intermediate_table tbl;
  try {
    fhicl::parse_document(in, policy, tbl);
  }
  catch (cet::exception const& e) {
    std::cerr << e.what() << '\n';
    return 0;
  }
  return 1;
} // main()
