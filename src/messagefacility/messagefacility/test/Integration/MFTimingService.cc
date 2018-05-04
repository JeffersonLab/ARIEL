//#define NDEBUG
#define ML_DEBUG // always enable debug

#include <cstdlib>
#include <iostream>

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/ParameterSetRegistry.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include "cetlib/filepath_maker.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

void
runModule(const std::string& modulename)
{
  mf::SetContextSinglet(modulename);

  // Post begin job
  mf::SetContextIteration("postBeginJob");
  mf::LogAbsolute("TimeReport")
    << "TimeReport> Report activated\n"
       "TimeReport> Report columns headings for events: "
       "eventnum runnum timetaken\n"
       "TimeReport> Report columns headings for modules: "
       "eventnum runnum modulelabel modulename timetaken";

  // Post end job
  mf::SetContextIteration("postEndJob");
  mf::LogAbsolute("TimeReport") // Changelog 1
    << "TimeReport> Time report complete in " << 0.0402123 << " seconds\n"
    << " Time Summary: \n"
    << " Min: " << 303 << "\n"
    << " Max: " << 5555 << "\n"
    << " Avg: " << 4000 << "\n";

  // Post event processing
  mf::SetContextIteration("postEventProcessing");
  mf::LogAbsolute("TimeEvent")
    << "TimeEvent> "
    << "run: 1   subRun: 2    event: 456 " << .0440404;

  // Post Module
  mf::SetContextIteration("postModule");
  mf::LogAbsolute("TimeModule")
    << "TimeModule> "
    << "run: 1   subRun: 2    event: 456 "
    << "someString " << modulename << " " << 0.04404;
}

int
main()
{

  const std::string config_string("messageFacility.fcl");
  cet::filepath_lookup_nonabsolute filepath("FHICL_FILE_PATH");

  fhicl::ParameterSet main_pset;
  try {
    // create an intermediate table from the input string
    make_ParameterSet(config_string, filepath, main_pset);
  }
  catch (cet::exception& e) {
    std::cerr << "ERROR: Failed to create a parameter set from an input "
                 "configuration string with exception "
              << e.what() << ".\n";
    std::cerr << "       Input configuration string follows:\n"
              << "------------------------------------"
              << "------------------------------------"
              << "\n";
    std::cerr << config_string << "\n";
    std::cerr << "------------------------------------"
              << "------------------------------------"
              << "\n";
    return 7003;
  }

  // Start MessageFacility Service
  mf::StartMessageFacility(main_pset.get<fhicl::ParameterSet>("message"));

  // Set module name for the main thread
  mf::SetApplicationName("MessageFacility");
  runModule("module1");
  runModule("module5");

  return 0;
}
