// vim: set sw=2 expandtab :
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include "cetlib/filepath_maker.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <string>

using namespace std;
using namespace std::string_literals;

int
main(int argc, char** argv)
{
  Catch::Session session;
  std::string fhiclConfigFileName;
  using namespace Catch::clara;
  // Our non-option argument, being non-optional, should be first.
  auto cli = Arg(fhiclConfigFileName, "config.fcl")("<fhicl-config-file>") |
             session.cli();

  session.cli(cli);

  int result = session.applyCommandLine(argc, argv);
  if (result == 0) {
    mf::SetIteration("JobSetup"s);
    fhicl::ParameterSet msg_ps;
    cet::filepath_maker fpm;
    fhicl::make_ParameterSet(fhiclConfigFileName, fpm, msg_ps);
    mf::StartMessageFacility(msg_ps);
    result = session.run();
  }
  return (result < 0xff ? result : 0xff);
}
