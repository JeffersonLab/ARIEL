#include "hep_concurrency/SharedResourceNames.h"

#include <atomic>
#include <sstream>

const std::string hep::concurrency::SharedResourceNames::kGEANT = "GEANT";
const std::string hep::concurrency::SharedResourceNames::kCLHEPRandomEngine =
  "CLHEPRandomEngine";
const std::string hep::concurrency::SharedResourceNames::kPythia6 = "Pythia6";
const std::string hep::concurrency::SharedResourceNames::kPythia8 = "Pythia8";
const std::string hep::concurrency::SharedResourceNames::kPhotos = "Photos";
const std::string hep::concurrency::SharedResourceNames::kTauola = "Tauola";
const std::string hep::concurrency::SharedResourceNames::kEvtGen = "EvtGen";
const std::string hep::concurrency::SharedResourceNames::kHerwig6 = "Herwig6";

static std::atomic<unsigned int> counter;

// See comments in header file for the purpose of this function.
std::string
hep::concurrency::uniqueSharedResourceName()
{
  std::stringstream ss;
  ss << "uniqueSharedResourceName" << counter.fetch_add(1);
  return ss.str();
}
