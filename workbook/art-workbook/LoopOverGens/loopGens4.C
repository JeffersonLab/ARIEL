//
// Make a multipage pdf file with the histograms found in loopGens3.root
//

{

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat("emruoi");
  gStyle->SetHistLineColor(kBlue);
  gROOT->ForceStyle();

  TFile* file = new TFile( "output/loopGens4.root");

  TH1D* hNGens;  file->GetObject("gens/hNGens",  hNGens);
  TH1D* hP;      file->GetObject("gens/hP",      hP);
  TH1D* hcz;     file->GetObject("gens/hcz",     hcz);
  TH1D* hphi;    file->GetObject("gens/hphi",    hphi);
  TH1D* hmass;   file->GetObject("gens/hmass",   hmass);
  TH1D* hx;      file->GetObject("gens/hx",      hx);
  TH1D* hy;      file->GetObject("gens/hy",      hy);
  TH1D* hz;      file->GetObject("gens/hz",      hz);

  TH1D* hNChildren; file->GetObject("gens/hNChildren",  hNChildren);

  TCanvas *canvas = new TCanvas("canvas", "Plots from loopGens4.root" );

  // Open multipage pdf file
  canvas->Print("output/loopGens4.pdf[");

  // Page 1
  canvas->Divide(2,2);

  canvas->cd(1);  hNGens->Draw("H9");
  canvas->cd(2);  hP->Draw("H9");
  canvas->cd(3);  hcz->Draw("H9");
  canvas->cd(4);  hphi->Draw("H9");

  canvas->Update();
  canvas->Print("output/loopGens4.pdf");

  // Page 2
  canvas->Clear();
  canvas->Divide(2,2);

  canvas->cd(1);  hmass->Draw("H9");
  canvas->cd(2);  hx->Draw("H9");
  canvas->cd(3);  hy->Draw("H9");
  canvas->cd(4);  hz->Draw("H9");

  canvas->Update();
  canvas->Print("output/loopGens4.pdf");

  // Page 3
  canvas->Clear();
  canvas->Divide(1,2);

  canvas->cd(1);  hNChildren->Draw("H9");

  canvas->Update();
  canvas->Print("output/loopGens4.pdf");

  // Close multipage PDF file.
  canvas->Print("output/loopGens4.pdf]");

}
