// ======================================================================
//
// Executable for dumping processed configuration files
//
// ======================================================================

#include "boost/program_options.hpp"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace fhicl;

int main(int argc, char* argv[])
{

  namespace bpo = boost::program_options;
  std::string input_file;
  std::string output_file;
  bpo::options_description desc("fhicl-dump [-c] <file>\nOptions");
  desc.add_options()
    ( "help,h", "produce this help message")
    ( "config,c", bpo::value<std::string>(&input_file), "input file" )
    ( "output,o", bpo::value<std::string>(&output_file), "output file (default is STDOUT)");

  bpo::positional_options_description p;
  p.add("config",-1);

  bpo::variables_map vm;
  try {
    bpo::store(bpo::command_line_parser(argc, argv).
               options(desc).positional(p).run(), vm);
    bpo::notify(vm);
  }
  catch (bpo::error& err) {
    std::cerr << "Error processing command line in " << argv[0]
              << ": " << err.what() << '\n';
    return 1;
  };
  if ( vm.count("help") ) {
    std::cout << desc << std::endl;
    return 2;
  }

  if ( !vm.count("config") ) {
    std::cout << "\nMissing input configuration file.\n\n"
              << desc
              << std::endl;
    return 3;
  }

  cet::filepath_lookup policy("FHICL_FILE_PATH");

  fhicl::intermediate_table tbl;
  fhicl::parse_document(input_file, policy, tbl);
  fhicl::ParameterSet pset;
  fhicl::make_ParameterSet(tbl, pset);

  // to_indented_string includes '\n' at the end.
  std::string const dump = pset.to_indented_string(0,false);

  if ( vm.count("output") ) {
    std::ofstream os( output_file );
    os << dump;
    os.close();
  }
  else
    std::cout << dump;
}
