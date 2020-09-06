//
//  Look at the contents of the GenParticleCollection; make both printout and
//  histograms.
//

#include "toyExperiment/MCDataProducts/GenParticleCollection.h"
#include "toyExperiment/PDT/PDT.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#ifdef ART_ROOT_IO
#include "art_root_io/TFileService.h"
#else
#include "art/Framework/Services/Optional/TFileService.h"
#endif

#include "TH1F.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <string>

namespace tex {

  class InspectGenParticles : public art::EDAnalyzer {

  public:
    explicit InspectGenParticles(fhicl::ParameterSet const& pset);

    void beginJob() override;
    void analyze(const art::Event& event) override;

  private:
    // Particle data table.
    art::ServiceHandle<PDT> _pdt;

    art::InputTag _gensTag;
    int _maxPrint;

    art::ServiceHandle<art::TFileService> _tfs;

    TH1F* _hNGens;
    TH1F* _hCharge;
    TH1F* _hQTot;
    TH1F* _hpPhi;
    TH1F* _hpKplus;
    TH1F* _hpKminus;
    TH1F* _hpOther;

    TH1F* _hczPhi;
    TH1F* _hczKplus;
    TH1F* _hczKminus;
    TH1F* _hczOther;

    TH1F* _hAngle;
    TH1F* _hMassCheck;

    int _printCount;

    void checkMass(GenParticle const&);
  };

}

tex::InspectGenParticles::InspectGenParticles(fhicl::ParameterSet const& pset)
  : art::EDAnalyzer(pset)
  , _pdt()
  ,

  _gensTag(pset.get<std::string>("genParticleTag"))
  , _maxPrint(pset.get<int>("maxPrint", 0))
  , _tfs(art::ServiceHandle<art::TFileService>())
  , _printCount(0)
{}

void
tex::InspectGenParticles::beginJob()
{
  _hNGens =
    _tfs->make<TH1F>("nGens", "Number of generated particles", 20, 0., 20.);
  _hCharge =
    _tfs->make<TH1F>("Charge", "Charge of generated particles", 5, -2., 2.);
  _hQTot = _tfs->make<TH1F>("QTot", "Total charge of the event", 5, -2., 2.);
  _hpPhi = _tfs->make<TH1F>(
    "pPhi", "Momentum of generated Phi; [MeV]", 100, 0., 2000.);
  _hpKplus =
    _tfs->make<TH1F>("pKplus", "Momentum of decay K+; [MeV]", 100, 0., 2000.);
  _hpKminus =
    _tfs->make<TH1F>("pKminus", "Momentum of decay K-; [MeV]", 100, 0., 2000.);
  _hpOther = _tfs->make<TH1F>(
    "pOther", "Momentum of other particles; [MeV]", 100, 0., 2000.);

  _hczPhi = _tfs->make<TH1F>(
    "czPhi", "cos(theta) of generated Phi; [MeV]", 100, -1., 1.);
  _hczKplus =
    _tfs->make<TH1F>("czKplus", "cos(theta) of decay K+; [MeV]", 100, -1., 1.);
  _hczKminus =
    _tfs->make<TH1F>("czKminus", "cos(theta) of decay K-; [MeV]", 100, -1., 1.);
  _hczOther = _tfs->make<TH1F>(
    "czOther", "cos(theta) of other particles; [MeV]", 100, -1., 1.);

  _hAngle = _tfs->make<TH1F>(
    "Angle", "Openning angle between K+ K-; [degrees]", 100, 0., 180.);
  _hMassCheck = _tfs->make<TH1F>(
    "MassCheck", "Mass of K+ K- pair; [MeV]", 40, 1000., 1040.);
}

void
tex::InspectGenParticles::analyze(const art::Event& event)
{

  // Get the generated particles from the event.
  auto gens = event.getValidHandle<GenParticleCollection>(_gensTag);

  // Printout, if required.
  bool doPrint = (++_printCount < _maxPrint);
  if (doPrint) {
    std::cout << "\nNumber of GenParticles in event " << event.id().event()
              << " is: " << gens->size() << std::endl;
  }

  _hNGens->Fill(gens->size());

  // Total charge of the generated event.
  int qtot(0.);

  // Loop over all of the generated particles.
  for (auto const& gen : *gens) {

    if (doPrint) {
      std::cout << gen << std::endl;
    }

    double p = gen.momentum().vect().mag();
    double cz = gen.momentum().vect().cosTheta();

    double q = _pdt->getById(gen.pdgId()).charge();
    qtot += q;

    _hCharge->Fill(q);

    if (gen.pdgId() == PDGCode::phi) {
      _hpPhi->Fill(p);
      _hczPhi->Fill(cz);
      checkMass(gen);

    } else if (gen.pdgId() == PDGCode::K_plus) {
      _hpKplus->Fill(p);
      _hczKplus->Fill(cz);

    } else if (gen.pdgId() == PDGCode::K_minus) {
      _hpKminus->Fill(p);
      _hczKminus->Fill(cz);

    } else {
      _hpOther->Fill(p);
      _hczOther->Fill(cz);
    }
  }

  _hQTot->Fill(qtot);
}

void
tex::InspectGenParticles::checkMass(const tex::GenParticle& gen)
{
  if (gen.children().size() != 2)
    return;

  CLHEP::Hep3Vector const& p1 = gen.child(0)->momentum().vect();
  CLHEP::Hep3Vector const& p2 = gen.child(1)->momentum().vect();

  double angle = p1.angle(p2) / CLHEP::degree;
  _hAngle->Fill(angle);

  CLHEP::HepLorentzVector psum =
    gen.child(0)->momentum() + gen.child(1)->momentum();
  _hMassCheck->Fill(psum.mag());
}

DEFINE_ART_MODULE(tex::InspectGenParticles)
