#ifndef critic_test_art_ServiceUsing_h
#define critic_test_art_ServiceUsing_h

#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "critic/test/art/Wanted.h"

namespace art::test {

  class ServiceUsing {

  public:
    ServiceUsing(fhicl::ParameterSet const&, art::ActivityRegistry&);

    int
    getCachedValue() const
    {
      return cached_value_;
    }

    bool
    postBeginJobCalled() const
    {
      return postBeginJobCalled_;
    }

  private:
    void postBeginJob();

    bool postBeginJobCalled_{false};

    int cached_value_{};

    ServiceHandle<Wanted> wanted_{};
  };

} // namespace art::test

DECLARE_ART_SERVICE(art::test::ServiceUsing, LEGACY)

#endif /* critic_test_art_ServiceUsing_h */

// Local Variables:
// mode: c++
// End:
