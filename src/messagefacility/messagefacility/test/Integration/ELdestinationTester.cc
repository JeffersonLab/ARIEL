// vim: set sw=2 expandtab :

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
#include <string>

namespace bpo = boost::program_options;

using namespace std;
using namespace std::string_literals;

namespace {

  void
  runModule(string const& modulename)
  {
    mf::SetModuleName(modulename);
    // Post begin job
    mf::SetIteration("postBeginJob"s);
    mf::LogAbsolute("TimeReport"s)
      << "TimeReport> Report activated\n"
         "TimeReport> Report columns headings for events: "
         "eventnum runnum timetaken\n"
         "TimeReport> Report columns headings for modules: "
         "eventnum runnum modulelabel modulename timetaken";
    // Post end job
    mf::SetIteration("postEndJob"s);
    mf::LogAbsolute("TimeReport"s)
      << "TimeReport> Time report complete in " << 0.0402123 << " seconds\n"
      << " Time Summary: \n"
      << " Min: " << 303 << "\n"
      << " Max: " << 5555 << "\n"
      << " Avg: " << 4000 << "\n";
    // Post event processing
    mf::SetIteration("postEventProcessing"s);
    mf::LogAbsolute("TimeEvent"s)
      << "TimeEvent> "
      << "run: 1   subRun: 2    event: 456 " << .0440404;
    // Post Module
    mf::SetIteration("postModule"s);
    mf::LogAbsolute("TimeModule"s)
      << "TimeModule> "
      << "run: 1   subRun: 2    event: 456 "
      << "someString " << modulename << " " << 0.04404;
    mf::LogSystem("system"s) << "This would be a major problem, I guess.";
  }

} // unnamed namespace

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
          << "  }\n\n";
  descstr << "Usage: " << boost::filesystem::path(argv[0]).filename().native()
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
    // create an intermediate table from the input string
    make_ParameterSet(config_string, filepath, main_pset);
  }
  catch (cet::exception const& e) {
    cerr << "ERROR: Failed to create a parameter set from an input "
            "configuration string with exception "
         << e.what() << ".\n";
    cerr << "       Input configuration string follows:\n"
         << "------------------------------------"
         << "------------------------------------"
         << "\n";
    cerr << config_string << "\n";
    cerr << "------------------------------------"
         << "------------------------------------"
         << "\n";
    return 7003;
  }
  // Start MessageFacility Service
  try {
    mf::StartMessageFacility(main_pset.get<fhicl::ParameterSet>("message"s),
                             "MessageFacility"s);
  }
  catch (mf::Exception const& e) {
    cerr << e.what() << '\n';
    return 4;
  }
  catch (fhicl::detail::validationException const& e) {
    cerr << e.what() << '\n';
    return 6;
  }
  catch (cet::exception const& e) {
    cerr << e.what() << '\n';
    return 7;
  }
  catch (...) {
    cerr << "Caught unknown exception from mf::StartMessageFacility\n";
    return 5;
  }
  // Set module name for the main thread
  mf::SetModuleName("MFTest"s);
  mf::SetIteration("pre-event"s);
  // Memory Check output
  mf::LogWarning("MemoryCheck"s)
    << "MemoryCheck: module G4:g4run VSIZE 1030.34 0 RSS 357.043 0.628906"s;
  mf::LogWarning("MemoryCheck"s)
    << "MemoryCheck: module G4:g4run VSIZE 1030.34 0 RSS 357.25 0.199219"s;
  mf::LogWarning("MemoryCheck"s)
    << "MemoryCheck: module G4:g4run VSIZE 1030.34 5.2 RSS 357.453 0.1875"s;
  mf::LogInfo linfo("info"s);
  linfo << " vint contains: "s;
  vector<int> vint{1, 2, 5, 89, 3};
  auto i = begin(vint);
  auto const e = end(vint);
  while (i != e) {
    linfo << *i;
    if (++i != e) {
      linfo << ", ";
    }
  }
  // Issue messages with different severity levels
  mf::LogError("err1"s) << "This is an ERROR message."s;
  mf::LogError("err2"s) << "This is an ERROR message."s;
  mf::LogWarning("warning"s) << "Followed by a WARNING message."s;
  // Switch context
  mf::SetIteration("pro-event"s);
  // Log Debugs
  for (int i = 0; i != 5; ++i) {
    mf::LogError("catError"s) << "Error information."s;
    mf::LogWarning("catWarning"s) << "Warning information."s;
    mf::LogInfo("catInfo"s) << "Info information."s;
    LOG_DEBUG("debug"s) << "DEBUG information."s;
  }
  // Test move operations
  {
    // normal macro
    auto logPROBLEM = LOG_PROBLEM("problem"s);
    logPROBLEM << "clever way to ensure persistence of temporary object"s;
    // macro w/ternary operator
    auto log = LOG_DEBUG("debug2"s) << "first line.\n"s;
    log << "second line."s;
  }
  runModule("module1"s);
  runModule("module5"s);
  mf::LogStatistics();
}
