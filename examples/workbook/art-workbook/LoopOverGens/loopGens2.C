//
// Draw three histograms on one page and write to a png file.
//

{

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat("emruoi");
  gStyle->SetHistLineColor(kBlue);
  gROOT->ForceStyle();

  TFile* file = new TFile( "output/loopGens2.root");

  TH1D* hNGens;  file->GetObject("gens/hNGens",  hNGens);
  TH1D* hP;      file->GetObject("gens/hP",      hP);
  TH1D* hcz;     file->GetObject("gens/hcz",     hcz);

  TCanvas *canvas = new TCanvas("canvas", "Plots from loopGens2.root" );

  canvas->Divide(2,2);

  canvas->cd(1);  hNGens->Draw("H9");
  canvas->cd(2);  hP->Draw("H9");
  canvas->cd(3);  hcz->Draw("H9");

  canvas->Update();
  canvas->Print("output/loopGens2.png");

}
