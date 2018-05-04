// vim: set sw=2 expandtab :
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <string>
#include <thread>

using namespace std;
using namespace std::string_literals;

void
anotherLogger()
{
  mf::SetModuleName("anotherLogger"s);
  mf::LogWarning("cat1|cat2"s)
    << "A WARNING message from the anotherLogger thread.";
  mf::LogDebug("cat1"s) << "The debug message in the anotherLogger thread";
  return;
}

int
main()
{
  fhicl::ParameterSet ps;
  string const psString{" statistics: [\"stats\"]"
                        " destinations: {"
                        "   console: { type: \"cout\" threshold: \"DEBUG\" }"
                        "   file: {"
                        "     type: \"file\" threshold: \"DEBUG\""
                        "     filename: \"mylog\""
                        "     append: false"
                        "   }"
                        " }"};
  fhicl::make_ParameterSet(psString, ps);
  // Start MessageFacility Service
  mf::StartMessageFacility(ps);
  // Set application name (use process name by default)
  mf::SetApplicationName("MF_Example"s);
  // Set module name for the main thread
  mf::SetModuleName("MF_main"s);
  mf::SetIteration("pre-event"s);
  // Start up another logger in a separate thread
  thread loggerThread(anotherLogger);
  // Issue messages with different severity levels
  mf::LogError("cat1|cat2"s) << "This is an ERROR message.";
  mf::LogWarning("cat3"s) << "Followed by a WARNING message.";
  // Switch iteration
  mf::SetIteration("pro-event"s);
  // Logs
  mf::LogError("cat2"s) << "Error information.";
  mf::LogWarning("cat3"s) << "Warning information.";
  mf::LogInfo("cat4"s) << "Info information.";
  mf::LogDebug("cat5"s) << "DEBUG information.";
  // Join with logger thread
  loggerThread.join();
  // Log statistics
  mf::LogStatistics();
  return 0;
}
