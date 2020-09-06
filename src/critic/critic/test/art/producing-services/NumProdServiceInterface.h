#ifndef critic_test_art_producing_services_NumProdServiceInterface_h
#define critic_test_art_producing_services_NumProdServiceInterface_h

#include "art/Framework/Core/ProducingService.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace arttest {
  class NumProdServiceInterface : public art::ProducingService {};
}

DECLARE_ART_SERVICE_INTERFACE(arttest::NumProdServiceInterface, LEGACY)

#endif /* critic_test_art_producing_services_NumProdServiceInterface_h */

// Local Variables:
// mode: c++
// End:
