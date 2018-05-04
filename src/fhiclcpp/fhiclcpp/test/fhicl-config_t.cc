#include "cetlib/filepath_maker.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace fhicl;

int
main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "ERROR: expect exactly one argument.\n";
    exit(1);
  }

  putenv(const_cast<char*>("FHICL_FILE_PATH=./test:."));
  cet::filepath_lookup policy("FHICL_FILE_PATH");

  std::string cfg_in(argv[1]);
  ParameterSet pset;
  make_ParameterSet(cfg_in, policy, pset);
  std::cout << pset.to_string() << std::endl;

  return 0;
}
