#include "canvas/Persistency/Provenance/ProcessHistory.h"
#include "canvas/Version/GetReleaseVersion.h"
#include "fhiclcpp/ParameterSetID.h"

#include <cassert>
#include <string>

int
main()
{
  art::ProcessHistory pnl1;
  // assert(pnl1 == pnl1);
  art::ProcessHistory pnl2;
  assert(pnl1 == pnl2);
  art::ProcessConfiguration const iHLT{
    "HLT", fhicl::ParameterSetID{}, art::getCanvasReleaseVersion()};
  art::ProcessConfiguration const iRECO{
    "RECO", fhicl::ParameterSetID{}, art::getCanvasReleaseVersion()};
  pnl2.push_back(iHLT);
  // assert(pnl1 != pnl2);
  art::ProcessHistory pnl3;
  pnl3.push_back(iHLT);
  pnl3.push_back(iRECO);

  art::ProcessHistoryID const id1{pnl1.id()};
  art::ProcessHistoryID const id2{pnl2.id()};
  art::ProcessHistoryID const id3{pnl3.id()};

  // assert(id1 != id2);
  // assert(id2 != id3);
  // assert(id3 != id1);

  art::ProcessHistory pnl4;
  pnl4.push_back(iHLT);
  art::ProcessHistoryID id4 = pnl4.id();
  assert(pnl4 == pnl2);
  assert(id4 == id2);

  art::ProcessHistory pnl5;
  pnl5 = pnl3;
  // assert(pnl5 == pnl3);
  // assert(pnl5.id() == pnl3.id());
}
