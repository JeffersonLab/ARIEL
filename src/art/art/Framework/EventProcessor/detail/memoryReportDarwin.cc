#include "art/Framework/EventProcessor/detail/memoryReport.h"
//#include "art/Utilities/DarwinProcMgr.h" // TODO
#include "messagefacility/MessageLogger/MessageLogger.h"

using mf::LogPrint;

void
art::detail::memoryReport()
{
  // Dummy implementation for Darwin
  //DarwinProcMgr procInfo{}; // TODO
  LogPrint("ArtSummary") << "MemReport  "
                         << "---------- Memory summary [base-10 MB] ------";
  LogPrint("ArtSummary") << "MemReport  VmPeak = " << 0.0
                         << " VmHWM = " << 0.0;
  LogPrint("ArtSummary") << "";
}
