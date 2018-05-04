#include "cetlib/filepath_maker.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/ParameterSetID.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace fhicl;
using namespace std;

int
main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "ERROR: expect exactly one filename!\n";
    exit(1);
  }
  intermediate_table tbl_ref;
  cet::filepath_lookup policy(".:");
  try {
    parse_document(argv[1], policy, tbl_ref);
  }
  catch (std::exception& e) {
    std::cerr << "ERROR: unable to parse FHiCL file "
              << "\"" << argv[1] << "\"\n"
              << e.what();
    exit(1);
  }
  ParameterSet pset_ref;
  make_ParameterSet(tbl_ref, pset_ref);
  string pset_string(pset_ref.to_string());
  intermediate_table tbl_test;
  parse_document(pset_string, tbl_test);
  ParameterSet pset_test;
  make_ParameterSet(tbl_test, pset_test);
  string pset_test_string(pset_test.to_string());
  if (pset_string != pset_test_string) {
    std::cerr << "ERROR: \n"
              << pset_string << "\n"
              << " !=\n"
              << pset_test_string << "\n";
    exit(1);
  }
}
