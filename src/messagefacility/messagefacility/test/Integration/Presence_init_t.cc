#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

int
main()
{
  fhicl::ParameterSet ps{};
  mf::StartMessageFacility(ps);
}
