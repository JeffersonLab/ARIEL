//
// Answer to the homework.
//

{

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat("emruoi");
  gStyle->SetHistLineColor(kBlue);
  gROOT->ForceStyle();

  TFile* file = new TFile( "output/countGens2.root");

  TH1D* hQ;    file->GetObject("count/hQ",    hQ);
  TH1D* hQSum; file->GetObject("count/hQSum", hQSum);

  TCanvas *canvas = new TCanvas("canvas", "Plots from countGens2.root" );

  canvas->Divide(1,2);

  canvas->cd(1);  hQ->Draw("H9");
  canvas->cd(2);  hQSum->Draw("H9");

  canvas->Update();
  canvas->Print("output/countGens2.pdf");

}
