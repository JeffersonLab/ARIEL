//
// Compare the six different momentum histograms.
//
// 1) Draw the histogram for Method 1.
//
// 2) For the other 6 methods, subtract the histograms:
//      histogram( Method N) - histogram(Method 1)
//    and plot the results
//

{

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat("emruoi");
  gStyle->SetHistLineColor(kBlue);
  gROOT->ForceStyle();

  TFile* file = new TFile( "output/loopGens3a.root");

  // Pointers to the histograms we want to work on.
  // Root-cint does not let us use: std::vector<TH1D*>.
  const int n=7;
  TH1D* h[n];

  // Get the histograms from the file.
  for ( int i=0; i!=n; ++i ){
     ostringstream os;
     os << "gens/hP" << i+1;
     file->GetObject( TString(os.str()), h[i]);
  }

  TCanvas *canvas = new TCanvas("canvas", "Plots from loopGens3a.root" );

  canvas->Divide(2,2);

  canvas->cd(1);
  h[0]->Draw("H9");

  for ( int i=1; i!=n; ++i ){
    int pad = i%4+1;
    canvas->cd(pad);

    // Copy of the histogram, with a new title.
    TH1D *htmp = new TH1D( *h[i] );
    ostringstream os;
    os << "Method " << i+1 << " - Method 1";
    htmp->SetTitle( TString( os.str() ) );

    // Subtract the histogram from Method 1.
    htmp->Add( h[0], -1.0);

    htmp->SetMinimum(-1.0);
    htmp->SetMaximum( 1.0);
    htmp->DrawCopy( "H9");

    // Clean up.
    delete htmp;

    // When the canvas is full, print it.
    if ( i==3 ) {
      canvas->Update();
      canvas->Print("output/loopGens3a_1.png");
      canvas->Clear();
      canvas->Divide(2,2);
    }
  }

  canvas->Update();
  canvas->Print("output/loopGens3a_2.png");

}
