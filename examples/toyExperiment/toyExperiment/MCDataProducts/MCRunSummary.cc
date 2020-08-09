//
// Some summary information about one art::Run of MC events.
// Used to illustrate adding a run scope data product at the end of run.
//

#include "toyExperiment/MCDataProducts/MCRunSummary.h"

tex::MCRunSummary::MCRunSummary():
  _nAlive(0), _nGood(0){
}

void
tex::MCRunSummary::increment ( size_t nAlive, size_t nGood){
  _nAlive += nAlive;
  _nGood  += nGood;
}

void
tex::MCRunSummary::clear(){
  _nAlive=0;
  _nGood=0;
}

std::ostream&
tex::operator<<(std::ostream& ost,
                const MCRunSummary& sum ){
  ost << "MC Run Summary: nAlive " << sum.nAlive()
      << " nGood: "                << sum.nGood();
  return ost;
}
