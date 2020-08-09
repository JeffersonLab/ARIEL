//
//  Make an invariant mass plot for reconstructed K+ K- pairs.
//

#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/PDT/PDT.h"
#include "toyExperiment/RecoDataProducts/FittedHelixDataCollection.h"
#include "toyExperiment/RecoDataProducts/RecoTrk.h"
#include "toyExperiment/Reconstruction/FittedHelix.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "TH1F.h"
#include "TNtuple.h"

#include "cetlib_except/exception.h"

#include <iostream>
#include <string>

namespace tex {

  class MassPlot : public art::EDAnalyzer {

  public:

    explicit MassPlot(fhicl::ParameterSet const& pset);

    void beginJob() override;
    void analyze(const art::Event& event) override;

  private:

    art::InputTag _fitsTag;
    int           _maxPrint;

    art::ServiceHandle<Geometry>          _geom;
    art::ServiceHandle<PDT>               _pdt;
    art::ServiceHandle<art::TFileService> _tfs;

    double _mka;
    double _mphi;

    int _printCount;

    TH1F* _hMass0;
    TH1F* _hMass1;
    TH1F* _hMass2;
    TH1F* _hSigM;
    TH1F* _hPull;

    TNtuple* _nt;
    static const size_t ntNVar;
  };

}

const size_t tex::MassPlot::ntNVar(7);

tex::MassPlot::MassPlot(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset),
  _fitsTag( pset.get<std::string>("fitsTag") ),
  _maxPrint( pset.get<int>("maxPrint",0) ),
  _geom(),
  _pdt(),
  _tfs(),
  _mka(_pdt->getById(PDGCode::K_plus).mass()),
  _mphi(_pdt->getById(PDGCode::phi).mass()),
  _printCount(0),
  _hMass0(0),
  _hMass1(0),
  _hMass2(0),
  _hSigM(0),
  _hPull(0),
  _nt(0){
}

void tex::MassPlot::beginJob(){

  _hMass0 = _tfs->make<TH1F>("Mass0", "Reconstructed mass;[MeV]",        100,  900., 3000. );
  _hMass1 = _tfs->make<TH1F>("Mass1", "Reconstructed mass;[MeV]",        100,  900., 1300. );
  _hMass2 = _tfs->make<TH1F>("Mass2", "Reconstructed mass;[MeV]",        100, 1010., 1030. );
  _hSigM  = _tfs->make<TH1F>("SigM",  "Reconstructed sigma(Mass);[MeV]", 100,    0.,   10. );
  _hPull  = _tfs->make<TH1F>("Pull",  "Mass (Reco-Gen)/sigma",           100,   -5.,    5. );

  _nt    = _tfs->make<TNtuple>("nt", "Mass Plot Ntuple", "mass:sigm:pull:p1:p2:cz1:cz2");

  if ( size_t(_nt->GetNvar()) != ntNVar ){
    throw cet::exception("HISTOGRAMS") << "Dimension mismatch between TNtuple and expectations in local code: \n"
                                       << "Local code: " << ntNVar
                                       << "   TNtuple: " << _nt->GetNvar() << "\n";
  }

}

void tex::MassPlot::analyze(const art::Event& event){

  ++_printCount;

  // Get the fitted helices from the event.
  auto fits = event.getValidHandle<FittedHelixDataCollection>(_fitsTag);

  // Buffer for filling the ntuple.
  float nt[ntNVar];

  // Loop over all unique pairs of oppositely charged tracks.
  for ( size_t i=0; i+1<fits->size(); ++i ) {

    FittedHelix fit1(fits->at(i));

    RecoTrk trk1( fit1.lorentzAtPoca(_mka),
                  fit1.lorentzAtPocaCov(_mka) );

    for ( size_t j=i+1; j<fits->size(); ++j){

      FittedHelix fit2(fits->at(j));

      // Skip a pair of tracks if they have the same sign of electric charge.
      if ( std::abs(fit1.q()+fit2.q()) > 0.5 ) continue;

      RecoTrk trk2( fit2.lorentzAtPoca(_mka), fit2.lorentzAtPocaCov(_mka) );

      // For the two track system, compute the invariant mass, the error and the pull from the MC truth.
      RecoTrk trk3 = trk1 + trk2;
      double m     = trk3.momentum().mag();
      double sigm  = trk3.sigmaMass();
      double mpull = (sigm > 0 ) ? (( m-_mphi)/sigm) : -10.;

      // Fill histograms and ntuples.
      _hMass0->Fill(m);
      _hMass1->Fill(m);
      _hMass2->Fill(m);
      _hSigM->Fill(sigm);
      _hPull->Fill(mpull);

      nt[0] = m;
      nt[1] = sigm;
      nt[2] = mpull;
      nt[3] = fit1.momAtPoca().mag();
      nt[4] = fit2.momAtPoca().mag();
      nt[5] = fit1.data().helix().cosTheta();
      nt[6] = fit2.data().helix().cosTheta();

      _nt->Fill(nt);

      // Printout when requested
      if ( _printCount <= _maxPrint ){
        std::cout << "2 Track combination in event: " << event.id()
                  << " mass: " << m
                  << std::endl;
      }

    }
  }

}

DEFINE_ART_MODULE(tex::MassPlot)
