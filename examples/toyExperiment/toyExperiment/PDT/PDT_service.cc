//
// Serve the Particle Data Table information.
//

#include "toyExperiment/PDT/PDT.h"
#include "toyExperiment/Utilities/ParameterSetFromFile.h"

#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Principal/Run.h"
#include "canvas/Persistency/Provenance/ModuleDescription.h"

#include "fhiclcpp/ParameterSet.h"

#include <iostream>
#include <iomanip>
#include <sstream>

tex::PDT::PDT( fhicl::ParameterSet const& pset,
               art::ActivityRegistry& ) :
  _pdtFile(pset.get<std::string>("pdtFile")),
  _verbosity(pset.get<int> ("verbosity",0))
{

  if ( _verbosity > 0 ){
    std::cout << "PDT: "
	      << " PDT file: "  << _pdtFile
	      << " verbosity: " << _verbosity
	      << std::endl;
  }

  // Copy particle data information from the text file to the map.
  ParameterSetFromFile document(_pdtFile);

  std::vector<fhicl::ParameterSet> parts =
    document.pSet().get<std::vector<fhicl::ParameterSet>>("particles");

  for ( auto const& part : parts ){

    std::string name    = part.get<std::string>("name");
    double      mass    = part.get<double>("mass");
    double      charge  = part.get<double>("charge");
    PDGCode::type pdgId = static_cast<PDGCode::type>(part.get<int>("pdgId"));

    _pdt[pdgId] = ParticleInfo(pdgId,mass,charge,name);
  }

  if ( _verbosity > 2 ){
    std::cout << "Contents of particle data table." << std::endl;
    print(std::cout);
  }

}

void
tex::PDT::print( std::ostream& ) const{
  for ( map_type::const_iterator i=_pdt.begin();
        i  != _pdt.end(); ++i ){
    std::cout << i->second << std::endl;
  }
}

tex::ParticleInfo const&
tex::PDT::getById( PDGCode::type id) const{
  map_type::const_iterator i = _pdt.find(id);
  if ( i == _pdt.end() ){
    throw cet::exception("PDT")
      << "Unrecognized pdg id code: " << id << "\n";
  }
  return i->second;
}

std::ostream& tex::operator<<(std::ostream& ost,
                              const tex::PDT& t ){
  t.print(ost);
  return ost;
}


DEFINE_ART_SERVICE(tex::PDT)
