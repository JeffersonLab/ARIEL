//
// Count the number of times each PDG Id appears.
// Print a table of the results when requested.
//

#include "art-workbook/UsePDTService/IdCounter.h"

#include "toyExperiment/PDT/PDT.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"

#include <iostream>
#include <iomanip>
#include <cmath>

tex::IdCounter::IdCounter():
  counts_(){
}

void tex::IdCounter::print( std::ostream& ost, outputStyle style) const{

  art::ServiceHandle<PDT> pdt;

  // No header; print name and count only.
  if ( style == minimal ){
    for ( auto i : counts_ ){
      ost << pdt->getById(i.first).name() << " " << i.second << std::endl;
    }
    return;
  }

  // Find information needed to format the output.
  size_t widthName(0);
  int    maxCount(0);
  int    maxId(0);
  for ( auto const& i : counts_ ){
    widthName = std::max ( widthName, pdt->getById(i.first).name().size() );
    maxCount  = std::max ( maxCount,  i.second );
    maxId     = std::max ( maxId,     std::abs(int(i.first)) );
  }

  // Width of the Id field.
  // +1 for the round up; +1 to allow for minus sign = +2
  int widthId    = static_cast<int>(std::log10(maxId))+2;

  // Width of the count field.
  int widthCount = static_cast<int>(std::log10(maxCount))+1;

  ost<< "\nSummary of Particle types found in this job" << std::endl;
  for ( auto i : counts_ ){
    ost << "Name: "    << std::setw(widthName)  << pdt->getById(i.first).name()
        << "  Id: "    << std::setw(widthId)    << i.first
        << "  Count: " << std::setw(widthCount) << i.second
        << std::endl;
  }
}
