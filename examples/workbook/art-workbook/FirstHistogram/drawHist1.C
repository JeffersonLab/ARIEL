//
// Root script to draw the histogram made by FirstHist1_module.cc
//

{

  // With this you can reinvoke the script without exiting root and restarting.
  gROOT->Reset();

  // Get rid of grey background (ugly for printing).
  gROOT->SetStyle("Plain");

  // Recommended content of statistics box:
  // Number of Entries, Mean, Rms, Underflows, Overflows, Integral within limits
  gStyle->SetOptStat("emruoi");

  // Open the input file that contains histogram.
  TFile* file = new TFile( "output/firstHist1.root");

  // Get pointer to the histogram.
  TH1D* hNGens(0);  file->GetObject("hist1/hNGens",  hNGens);

  // Open a new canvas on the screen.
  TCanvas *canvas = new TCanvas("canvas", "Plots from Firsthist1.root" );

  // "H9": draw outline histogram ("H") in high resolution mode (9)
  hNGens->Draw("H9");

  canvas->Update();
  canvas->Print("output/NumberGenerated.pdf");

}
