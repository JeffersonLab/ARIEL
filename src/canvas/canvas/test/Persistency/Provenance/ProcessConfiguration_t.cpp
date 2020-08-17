#include "canvas/Persistency/Provenance/ProcessConfiguration.h"

#include <cassert>

using namespace std;

int
main()
{
  art::ProcessConfiguration pc1;
  assert(pc1 == pc1);
  art::ProcessConfiguration pc2;
  assert(pc1 == pc2);
#if 0
  pc2.processName() = "reco2";
  art::ProcessConfiguration pc3;
  pc3.processName() = "reco3";

  art::ProcessConfigurationID id1 = pc1.id();
  art::ProcessConfigurationID id2 = pc2.id();
  art::ProcessConfigurationID id3 = pc3.id();

  assert(id1 != id2);
  assert(id2 != id3);
  assert(id3 != id1);

  art::ProcessConfiguration pc4;
  pc4.processName() = "reco2";
  art::ProcessConfigurationID id4 = pc4.id();
  assert(pc4 == pc2);
  assert (id4 == id2);
#endif // 0
}
