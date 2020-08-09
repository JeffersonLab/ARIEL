//
//  First exercise in the ParameterSets directory
//   - Introduce reading from a parameter set and printing parameter sets
//

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include <iostream>
#include <string>

namespace tex {

  class PSet01 : public art::EDAnalyzer {

  public:

    explicit PSet01(fhicl::ParameterSet const& );

    void analyze(art::Event const& event) override;

  };

}

tex::PSet01::PSet01(fhicl::ParameterSet const& pset ):
  art::EDAnalyzer(pset){

  std::string            a = pset.get<std::string>("a");
  int                    b = pset.get<int>   ("b");
  double                 c = pset.get<double>("c");
  bool                   d = pset.get<bool>  ("d");
  std::vector<int>       e = pset.get<std::vector<int>>("e");
  fhicl::ParameterSet    f = pset.get<fhicl::ParameterSet>("f");

  int                   fa = f.get<int>("a");
  int                   fb = f.get<int>("b");

  std::string module_type  = pset.get<std::string>("module_type");
  std::string module_label = pset.get<std::string>("module_label");

  std::cout << "\n--------------------\nPart 1:\n";
  std::cout << "a : " << a << std::endl;
  std::cout << "b : " << b << std::endl;
  std::cout << "c : " << c << std::endl;
  std::cout << "d : " << d << std::endl;

  std::cout << "e :";
  for ( int i: e ){
    std::cout << " " << i;
  }
  std::cout << std::endl;

  std::cout << "f.a : " << fa << std::endl;
  std::cout << "f.b : " << fb << std::endl;

  std::cout << "module_type:  "  << module_type  << std::endl;
  std::cout << "module_label: " << module_label << std::endl;

  std::cout << "\n--------------------\nPart 2:\n";
  std::cout << "f as string:          "  << f.to_string() << std::endl;
  std::cout << "f as indented-string:\n" << f.to_indented_string() << std::endl;

  std::cout << "\n--------------------\nPart 3:\n";
  std::cout << "pset:\n" << pset.to_indented_string() << std::endl;

}

void tex::PSet01::analyze(art::Event const& ){
}

DEFINE_ART_MODULE(tex::PSet01)
