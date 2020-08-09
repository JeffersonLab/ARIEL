//
// - ROOT-based 3D event display for ART toy experiment.  Requires
//   EvtDisplayUtils, NavState, and EvtDisplayService.
//

// toyExperiment includes
#include "toyExperiment/Geometry/Geometry.h"
#include "toyExperiment/MCDataProducts/IntersectionCollection.h"
#include "toyExperiment/MCDataProducts/GenParticleCollection.h"
#include "toyExperiment/PDT/PDT.h"

// art includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// ROOT includes
// ... libCore
#include <TApplication.h>
#include <TString.h>
#include <TSystem.h>
// ... libRIO
#include <TFile.h>
// ... libGui
#include <TGString.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGTextEntry.h>
#include <TGTextView.h>
#include <TGLayout.h>
#include <TGTab.h>
#include <TG3DLine.h>
// ... libGeom
#include <TGeoManager.h>
#include <TGeoTube.h>
#include <TGeoCompositeShape.h>
#include <TGeoBoolNode.h>
// ... libEG
#include <TParticle.h>
// ... libRGL
#include <TGLViewer.h>
// ... libEve
#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TEveBrowser.h>
#include <TEveGeoNode.h>
#include <TEveViewer.h>
#include <TEveScene.h>
#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>
#include <TEvePointSet.h>
#include <TEveTrack.h>
#include <TEveTrackPropagator.h>

//#include <sstream>

#include "EvtDisplayUtils.h"

// ... Anonymous namespace for helpers.
namespace {

  // ... Helper for setting color and transparency of detector objects
  void setRecursiveColorTransp(TGeoVolume *vol, Int_t color, Int_t transp)
  {
     if(color>=0)vol->SetLineColor(color);
     if(transp>=0)vol->SetTransparency(transp);
     Int_t nd = vol->GetNdaughters();
     for (Int_t i=0; i<nd; i++) {
        setRecursiveColorTransp(vol->GetNode(i)->GetVolume(), color, transp);
     }
  }

  // ... Helper for drawing hit as a TEvePointSet with specified color, markersize, and hit index
  void drawHit(const std::string &pstr, Int_t mColor, Int_t mSize, Int_t n, const tex::Intersection &hit, TEveElementList *list)
  {
    std::string hstr=" hit %d";
    std::string dstr=" hit# %d\nLayer: %d";
    std::string strlst=pstr+hstr;
    std::string strlab=pstr+dstr;

    TEvePointSet* h = new TEvePointSet(Form(strlst.c_str(),n));
    h->SetTitle(Form(strlab.c_str(),n,hit.shell()));
    h->SetNextPoint(hit.position().x()*0.1,hit.position().y()*0.1,hit.position().z()*0.1);
    h->SetMarkerColor(mColor);
    h->SetMarkerSize(mSize);
    list->AddElement(h);
  }
}

namespace tex {

  class EventDisplay3D : public art::EDAnalyzer {

  public:

    explicit EventDisplay3D(fhicl::ParameterSet const& pset);

    void beginJob() override;
    void endJob()   override;
    void beginRun( const art::Run& run ) override;
    void analyze(const art::Event& event) override;

  private:

    // Set by parameter set variables.
    art::InputTag   gensTag_;
    bool            drawGenTracks_;
    bool            drawHits_;
    Double_t        hitMarkerSize_;
    Double_t        trkMaxR_;
    Double_t        trkMaxZ_;
    Double_t        trkMaxStepSize_;
    Double_t        camRotateCenterH_;
    Double_t        camRotateCenterV_;
    Double_t        camDollyDelta_;

    art::ServiceHandle<Geometry>          geom_;
    art::ServiceHandle<PDT>               pdt_;

    std::unique_ptr<tex::EvtDisplayUtils>visutil_;
    TEveGeoShape* fSimpleGeom;

    TEveViewer *fXYView;
    TEveViewer *fRZView;
    TEveProjectionManager *fXYMgr;
    TEveProjectionManager *fRZMgr;
    TEveScene *fDetXYScene;
    TEveScene *fDetRZScene;
    TEveScene *fEvtXYScene;
    TEveScene *fEvtRZScene;

    TGTextEntry      *fTeRun,*fTeEvt;
    TGLabel          *fTlRun,*fTlEvt;

    TEveTrackList *fTrackList;
    TEveElementList *fHitsList;

    void makeNavPanel();
    
  };

}

tex::EventDisplay3D::EventDisplay3D(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset),
  gensTag_        ( pset.get<std::string>("genParticleTag") ),
  drawGenTracks_  ( pset.get<bool>       ("drawGenTracks",true) ),
  drawHits_       ( pset.get<bool>       ("drawHits",true) ),
  hitMarkerSize_  ( pset.get<Double_t>   ("hitMarkerSize", 2.) ),
  trkMaxR_        ( pset.get<Double_t>   ("trkMaxR", 100.) ),
  trkMaxZ_        ( pset.get<Double_t>   ("trkMaxZ", 50.) ),
  trkMaxStepSize_ ( pset.get<Double_t>   ("trkMaxStepSize", 1.) ), // ROOT default is 20
  camRotateCenterH_ ( pset.get<Double_t>   ("camRotateCenterH", 0.26) ),
  camRotateCenterV_ ( pset.get<Double_t>   ("camRotateCenterV",-2.  ) ),
  camDollyDelta_    ( pset.get<Double_t>   ("camDollyDelta",500.) ),
  geom_(art::ServiceHandle<Geometry>()),pdt_(),
  visutil_(new tex::EvtDisplayUtils()),
  fSimpleGeom(0),
  fXYView(0),fRZView(0),fXYMgr(0),fRZMgr(0),
  fDetXYScene(0),fDetRZScene(0),fEvtXYScene(0),fEvtRZScene(0),
  fTeRun(0),fTeEvt(0),
  fTlRun(0),fTlEvt(0),
  fTrackList(0),fHitsList(0){

  if ( trkMaxStepSize_ < 0.1 )trkMaxStepSize_ = 0.1;

}

void tex::EventDisplay3D::makeNavPanel()
{
  // Create control panel for event navigation
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    TEveBrowser* browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kLeft); // insert nav frame as new tab in left pane

  TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
  frmMain->SetWindowName("EVT NAV");
  frmMain->SetCleanup(kDeepCleanup);

  TGHorizontalFrame* navFrame = new TGHorizontalFrame(frmMain);
  TGVerticalFrame* evtidFrame = new TGVerticalFrame(frmMain);
  {
    TString icondir(TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")) );
    TGPictureButton* b = 0;

    // ... Create back button and connect to "PrevEvent" rcvr in visutils
    b = new TGPictureButton(navFrame, gClient->GetPicture(icondir + "GoBack.gif"));
    navFrame->AddFrame(b);
    b->Connect("Clicked()", "tex::EvtDisplayUtils", visutil_.get(), "PrevEvent()");

    // ... Create forward button and connect to "NextEvent" rcvr in visutils
    b = new TGPictureButton(navFrame, gClient->GetPicture(icondir + "GoForward.gif"));
    navFrame->AddFrame(b);
    b->Connect("Clicked()", "tex::EvtDisplayUtils", visutil_.get(), "NextEvent()");

    // ... Create run num text entry widget and connect to "GotoEvent" rcvr in visutils
    TGHorizontalFrame* runoFrame = new TGHorizontalFrame(evtidFrame);
    fTlRun = new TGLabel(runoFrame,"Run Number");
    fTlRun->SetTextJustify(kTextLeft);
    fTlRun->SetMargins(5,5,5,0);
    runoFrame->AddFrame(fTlRun);
    
    fTeRun = new TGTextEntry(runoFrame, visutil_->fTbRun = new TGTextBuffer(5), 1);
    visutil_->fTbRun->AddText(0, "1");
    fTeRun->Connect("ReturnPressed()","tex::EvtDisplayUtils", visutil_.get(),"GotoEvent()");
    runoFrame->AddFrame(fTeRun,new TGLayoutHints(kLHintsExpandX));

    // ... Create evt num text entry widget and connect to "GotoEvent" rcvr in visutils
    TGHorizontalFrame* evnoFrame = new TGHorizontalFrame(evtidFrame);
    fTlEvt = new TGLabel(evnoFrame,"Evt Number");
    fTlEvt->SetTextJustify(kTextLeft);
    fTlEvt->SetMargins(5,5,5,0);
    evnoFrame->AddFrame(fTlEvt);

    fTeEvt = new TGTextEntry(evnoFrame, visutil_->fTbEvt = new TGTextBuffer(5), 1);
    visutil_->fTbEvt->AddText(0, "1");
    fTeEvt->Connect("ReturnPressed()","tex::EvtDisplayUtils", visutil_.get(),"GotoEvent()");
    evnoFrame->AddFrame(fTeEvt,new TGLayoutHints(kLHintsExpandX));

    // ... Add horizontal run & event number subframes to vertical evtidFrame
    evtidFrame->AddFrame(runoFrame,new TGLayoutHints(kLHintsExpandX));
    evtidFrame->AddFrame(evnoFrame,new TGLayoutHints(kLHintsExpandX));

    // ... Add navFrame and evtidFrame to MainFrame
    frmMain->AddFrame(navFrame);
    TGHorizontal3DLine *separator = new TGHorizontal3DLine(frmMain);
    frmMain->AddFrame(separator, new TGLayoutHints(kLHintsExpandX));
    frmMain->AddFrame(evtidFrame);

    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding();
    browser->SetTabTitle("Event Nav", 0);
  }
}

void tex::EventDisplay3D::beginJob(){

  // Initialize global Eve application manager (return gEve)
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  TEveManager::Create();

  // Create detector and event scenes for ortho views
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fDetXYScene = gEve->SpawnNewScene("Det XY Scene", "");
  fDetRZScene = gEve->SpawnNewScene("Det RZ Scene", "");
  fEvtXYScene = gEve->SpawnNewScene("Evt XY Scene", "");
  fEvtRZScene = gEve->SpawnNewScene("Evt RZ Scene", "");

  // Create XY/RZ projection mgrs, draw projected axes, & add them to scenes
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fXYMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
  TEveProjectionAxes* axes_xy = new TEveProjectionAxes(fXYMgr);
  fDetXYScene->AddElement(axes_xy);

  fRZMgr = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
  TEveProjectionAxes* axes_rz = new TEveProjectionAxes(fRZMgr);
  fDetRZScene->AddElement(axes_rz);

  // Create side-by-side ortho XY & RZ views in new tab & add det/evt scenes
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  TEveWindowSlot *slot = 0;
  TEveWindowPack *pack = 0;

  slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
  pack = slot->MakePack();
  pack->SetElementName("Ortho Views");
  pack->SetHorizontal();
  pack->SetShowTitleBar(kFALSE);

  pack->NewSlot()->MakeCurrent();
  fXYView = gEve->SpawnNewViewer("XY View", "");
  fXYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  fXYView->AddScene(fDetXYScene);
  fXYView->AddScene(fEvtXYScene);

  pack->NewSlot()->MakeCurrent();
  fRZView = gEve->SpawnNewViewer("RZ View", "");
  fRZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  fRZView->AddScene(fDetRZScene);
  fRZView->AddScene(fEvtRZScene);

  gEve->GetBrowser()->GetTabRight()->SetTab(0);

  // Create navigation panel
  // ~~~~~~~~~~~~~~~~~~~~~~~~
  makeNavPanel();

  // Add new Eve event into the "Event" scene and make it the current event
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // (Subsequent elements added using "AddElements" will be added to this event)
  gEve->AddEvent(new TEveEventManager("Event", "Toy Detector Event"));

  // ... Set up initial camera orientation in main 3d view
  //   - rotate camera by "camRotateCenterH_" radians about horizontal going through
  //     center, followed by "camRotateCenterV_" radians about vertical going through
  //     center
  //   - move camera by "camDollyDelta_" units towards(+'ve) or away from(-'ve) center,
  //     combination of two boolean args controls sensitivity.
  TGLViewer *glv = gEve->GetDefaultGLViewer();
  glv->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
  glv->CurrentCamera().RotateRad(camRotateCenterH_,camRotateCenterV_);
  glv->CurrentCamera().Dolly(camDollyDelta_,kFALSE,kFALSE);

}

void tex::EventDisplay3D::beginRun( const art::Run& ){

  if(gGeoManager){
    gGeoManager->GetListOfNodes()->Delete();
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
  }
  gEve->GetGlobalScene()->DestroyElements();
  fDetXYScene->DestroyElements();
  fDetRZScene->DestroyElements();

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Using the detector specifications provided by the geometry service, create
  // a TGeoCompositeShape for drawing the detector in the main 3D view and an 
  // TEveElementList for drawing the detector in the 2D orthographic views
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  TGeoShape *composite=0;
  TEveElementList *orthodet = new TEveElementList("OrthoDet");
  std::string layerid="Layer %d";
  Int_t i=0;
  for ( auto const& shell : geom_->tracker().shells() ){
    Double_t dz = 0.2*shell.halfLength();
    Double_t rmin = 0.1*shell.radius() ;
    Double_t rmax = rmin+0.1*shell.thickness();
    if(i==0){
      composite = new TGeoTube(Form(layerid.c_str(),i),rmin,rmax,dz); 
    }else{
      TGeoShape *gs = new TGeoTube(Form(layerid.c_str(),i),rmin,rmax,dz); 
      TGeoBoolNode *bn = new TGeoUnion(composite,gs);
      composite = new TGeoCompositeShape("TGeoCompositeShape", bn);
    }
    TEveGeoShape *egs = new TEveGeoShape(Form(layerid.c_str(),i));
    egs->SetShape(new TGeoTube(rmin, rmax, dz));
    egs->SetMainColor(kPink+7);
    orthodet->AddElement(egs);
    i++;
  }

  // Draw the 3D detector using the composite shape created above
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // ... Define some materials
  TGeoMaterial *matVac = new TGeoMaterial("Vac", 0,0,0);
  TGeoMaterial *matSi = new TGeoMaterial("Si", 28.085,14,2.33);

  // ... Define some media
  TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVac);
  TGeoMedium *Si = new TGeoMedium("Silicon",2, matSi);

  // ... Define top volume as an empty box
  TGeoVolume *topvol = gGeoManager->MakeBox("Detector", Vacuum, 1000., 1000., 1000.);
  gGeoManager->SetTopVolume(topvol);

  // ... Create tracker out of Silicon using the composite shape defined above
  TGeoVolume *tracker = new TGeoVolume("Tracker",composite, Si);
  tracker->SetVisLeaves(kTRUE);
  topvol->AddNode(tracker, 1, new TGeoTranslation(0,0,0));
  gGeoManager->CloseGeometry();

  TGeoNode* topnode = gGeoManager->GetTopNode();
  TEveGeoTopNode* etopnode = new TEveGeoTopNode(gGeoManager, topnode);
  etopnode->SetVisLevel(4);
  etopnode->GetNode()->GetVolume()->SetVisibility(kFALSE);

  // ... Use helper to recursively make inner/outer tracker descendants 
  //     transparent & set custom colors
  setRecursiveColorTransp(etopnode->GetNode()->GetVolume(), kCyan-10, 70);

  // ... Add static detector geometry to global scene
  gEve->AddGlobalElement(etopnode);

  // Draw the 2D projections using the EVE element list created above 
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // .... Add the EVE element list to the global scene first
  gEve->AddGlobalElement(orthodet);

  // ... Import elements of the list into the projected views
  fXYMgr->ImportElements(orthodet, fDetXYScene);
  fRZMgr->ImportElements(orthodet, fDetRZScene);

  // ... Turn OFF rendering of duplicate detector in main 3D view
  gEve->GetGlobalScene()->FindChild("OrthoDet")->SetRnrState(kFALSE);

  // ... Turn ON rendering of detector in RPhi and RZ views
  fDetXYScene->FindChild("OrthoDet [P]")->SetRnrState(kTRUE);
  fDetRZScene->FindChild("OrthoDet [P]")->SetRnrState(kTRUE);

}

void tex::EventDisplay3D::analyze(const art::Event& event ){

  // ... Update the run and event numbers in the TGTextEntry widgets in the Navigation panel
  std::ostringstream sstr;
  sstr << event.id().run();
  visutil_->fTbRun->Clear();
  visutil_->fTbRun->AddText(0,sstr.str().c_str());
  gClient->NeedRedraw(fTeRun);

  sstr.str("");
  sstr << event.id().event();
  visutil_->fTbEvt->Clear();
  visutil_->fTbEvt->AddText(0,sstr.str().c_str());
  gClient->NeedRedraw(fTeEvt);

  // ... Delete visualization structures associated with previous event
  gEve->GetViewers()->DeleteAnnotations();
  gEve->GetCurrentEvent()->DestroyElements();

  // Draw the detector hits
  // ~~~~~~~~~~~~~~~~~~~~~~~
  if (drawHits_) {
    std::vector<art::Handle<IntersectionCollection>> hitsHandles;
    event.getManyByType(hitsHandles);

    if (fHitsList == 0) {
      fHitsList = new TEveElementList("Hits"); 
      fHitsList->IncDenyDestroy();              // protect element against destruction
    }
    else {
      fHitsList->DestroyElements();             // destroy children of the element
    }

    TEveElementList* KpHitsList  = new TEveElementList("K+ Hits"); 
    TEveElementList* KmHitsList  = new TEveElementList("K- Hits"); 
    TEveElementList* BkgHitsList = new TEveElementList("Bkg Hits"); 

    int ikp=0,ikm=0,ibkg=0;
    for ( auto const& handle: hitsHandles ){
      for ( auto const& hit: *handle ){
        if ( hit.genTrack()->pdgId() == PDGCode::K_plus ){
          drawHit("K+",kGreen,hitMarkerSize_,ikp++,hit,KpHitsList);
        } else if ( hit.genTrack()->pdgId() == PDGCode::K_minus ){
          drawHit("K-",kYellow,hitMarkerSize_,ikm++,hit,KmHitsList);
        } else{
          drawHit("Bkg",kViolet+1,hitMarkerSize_,ibkg++,hit,BkgHitsList);
        }
      }
    }
    fHitsList->AddElement(KpHitsList);  
    fHitsList->AddElement(KmHitsList);  
    fHitsList->AddElement(BkgHitsList);  
    gEve->AddElement(fHitsList);
  }

  // Draw the generated tracks as helices in a uniform axial field
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (drawGenTracks_) {
    auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);

    if (fTrackList == 0) {
      fTrackList = new TEveTrackList("Tracks"); 
      fTrackList->SetLineWidth(4);
      fTrackList->IncDenyDestroy();                 // protect element against destruction
    }
    else {
      fTrackList->DestroyElements();                // destroy children of the element
    }

    TEveTrackPropagator* trkProp = fTrackList->GetPropagator();
    trkProp->SetMagField(-geom_->bz()*1000.);
    trkProp->SetMaxR(trkMaxR_);
    trkProp->SetMaxZ(trkMaxZ_);
    trkProp->SetMaxStep(trkMaxStepSize_);

    int mcindex=-1;
    for ( auto const& gen: *gens){
      mcindex++;
      // ... Skip tracks decayed in the generator.
      if ( gen.hasChildren() ) continue;
      TParticle mcpart;
      mcpart.SetMomentum(gen.momentum().px(),gen.momentum().py(),gen.momentum().pz(),gen.momentum().e());
      mcpart.SetProductionVertex(gen.position().x()*0.1,gen.position().y()*0.1,gen.position().z()*0.1,0.);
      mcpart.SetPdgCode(gen.pdgId());
      TEveTrack* track = new TEveTrack(&mcpart,mcindex,trkProp);
      track->SetIndex(0);
      track->SetStdTitle();
      track->SetAttLineAttMarker(fTrackList);
      if ( gen.pdgId() == PDGCode::K_plus ){
        track->SetMainColor(kGreen);
      } else if ( gen.pdgId() == PDGCode::K_minus ){
        track->SetMainColor(kYellow);
      } else {
        track->SetMainColor(kViolet+1);
      }
      fTrackList->AddElement(track);
    }
    fTrackList->MakeTracks();
    gEve->AddElement(fTrackList);
  }
  
  // Import event into ortho views and apply projections
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  TEveElement* currevt = gEve->GetCurrentEvent();

  fEvtXYScene->DestroyElements();
  fXYMgr->ImportElements(currevt, fEvtXYScene);

  fEvtRZScene->DestroyElements();
  fRZMgr->ImportElements(currevt, fEvtRZScene);

} // end tex::EventDisplay3D::analyze

void tex::EventDisplay3D::endJob(){

}

DEFINE_ART_MODULE(tex::EventDisplay3D)
