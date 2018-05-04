//#define NDEBUG
#define ML_DEBUG // always enable debug

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "cetlib/filepath_maker.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/types/detail/validationException.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/Utilities/exception.h"

#include <cstdlib>
#include <iostream>

namespace bpo = boost::program_options;

using namespace std;

int
main(int argc, char* argv[])
{
  ostringstream descstr;
  descstr << "\nELdestinationTester is a sample program that can be used\n"
          << "to test how messages are logged using different destinations\n"
          << "and configurations.  This can be particularly helpful for\n"
          << "users who wish to supply their own plugin destinations.\n\n"
          << "The configuration file should look like\n"
          << "\n"
          << "  message: {\n"
          << "    destinations: {\n"
          << "      dest1: {\n"
          << "        ...\n"
          << "      }\n"
          << "      ...\n"
          << "    }\n"
          << "  }\n\n"
          << "Usage: " << boost::filesystem::path(argv[0]).filename().native()
          << " -c <config-file> \n\n"
          << "Allowed options";
  bpo::options_description desc{descstr.str()};
  desc.add_options()("config,c", bpo::value<string>(), "Configuration file.")(
    "help,h", "produce help message");
  bpo::variables_map vm;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
    bpo::notify(vm);
  }
  catch (bpo::error const& e) {
    cerr << "Exception from command line processing in " << argv[0] << ": "
         << e.what() << "\n";
    return 2;
  }
  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  if (!vm.count("config")) {
    cerr << "ERROR: No configuration specified ( -c <config_file> )" << endl;
    return 3;
  }
  string const config_string = vm["config"].as<string>();
  cet::filepath_lookup_nonabsolute filepath{"FHICL_FILE_PATH"};
  fhicl::ParameterSet main_pset;
  try {
    make_ParameterSet(config_string, filepath, main_pset);
  }
  catch (cet::exception const& e) {
    cerr << "ERROR: Failed to create a parameter set from an input "
         << "configuration string with exception:\n"
         << e.what() << "\n\n"
         << "Input configuration string was:\n\n"
         << config_string << "\n";
    return 7003;
  }
  try {
    mf::StartMessageFacility(main_pset.get<fhicl::ParameterSet>("message"),
                             string("MessageFacility"));
  }
  catch (mf::Exception const& e) {
    cerr << e.what() << endl;
    return 4;
  }
  catch (fhicl::detail::validationException const& e) {
    cerr << e.what() << endl;
    return 6;
  }
  catch (cet::exception const& e) {
    cerr << e.what() << endl;
    return 7;
  }
  catch (...) {
    cerr << "Caught unknown exception from mf::StartMessageFacility" << endl;
    return 5;
  }
  LOG_DEBUG("WildcardDebug_t") << "Testing";
}
