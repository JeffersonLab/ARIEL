//
// Second exercise in the ParameterSets directory
//  - Hold parameters as member data
//  - Introduce colon initializer syntax
//

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <vector>

namespace tex {

  class PSet02 : public art::EDAnalyzer {

  public:

    explicit PSet02(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  private:

    int    b_;
    double c_;
    fhicl::ParameterSet f_;

  };

}

tex::PSet02::PSet02(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset),
  b_(pset.get<int>("b")),
  c_(pset.get<double>("c")),
  f_(pset.get<fhicl::ParameterSet>("f")){
}

void tex::PSet02::analyze(art::Event const& event ){

  std::cout << "Event number: " << event.id()
            << "  b: " << b_
            << "  c: " << c_
            << "  f: " << f_.to_string()
            << std::endl;


}

DEFINE_ART_MODULE(tex::PSet02)
