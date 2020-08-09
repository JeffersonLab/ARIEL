#ifndef IterativeAlgDevel_ParticleInfoCollection_h
#define IterativeAlgDevel_ParticleInfoCollection_h

#include "fhiclcpp/ParameterSet.h"
#include "toyExperiment/DataProducts/PDGCode.h"
#include "toyExperiment/PDT/ParticleInfo.h"
#include "toyExperiment/Utilities/ParameterSetFromFile.h"

#include <map>
#include <vector>

namespace helper {

  using PDGCode      = tex::PDGCode;
  using ParticleInfo = tex::ParticleInfo;

  class ParticleInfoCollection {
  public:

    ParticleInfoCollection(std::string const& filename)
    {
      tex::ParameterSetFromFile doc{filename};

      for ( auto const& part : doc.pSet().get<std::vector<fhicl::ParameterSet>>("particles") ){
        std::string   const name   = part.get<std::string>("name");
        double        const mass   = part.get<double>("mass");
        double        const charge = part.get<double>("charge");
        PDGCode::type const pdgId  = static_cast<PDGCode::type>(part.get<int>("pdgId"));

        pdt_[pdgId] = ParticleInfo{pdgId,mass,charge,name};
      }
    }

    ParticleInfo getById( PDGCode::type const id ) const {
      auto result = pdt_.find(id);
      if ( result == pdt_.cend() )
        throw std::range_error("Particle not found in map.");
      return result->second;
    }

  private:
    std::map<PDGCode::type,ParticleInfo> pdt_;

  };

}

#endif
