//
// Draw the particle counts as a histogram with text labels.
//

{

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat("");
  gStyle->SetHistLineColor(kBlue);
  gROOT->ForceStyle();

  // The bin contents and labels.
  vector<double>  counts;
  vector<TString> labels;

  // Read the text file to fill counts and labels.
  ifstream in("output/countGens3.txt");
  while (in){
    TString label;
    int count;
    in >> label >> count;
    if ( !in ) break;
    counts.push_back(count);
    labels.push_back(label);
  }

  TCanvas *canvas = new TCanvas("canvas", "Plots from countGens3.txt" );

  // Create an fill the histogram and set the bin labels.
  TH1D *h = new TH1D ( "h", "Count of Generated Particles",
                       counts.size(), 0., counts.size() );

  for ( int i=0; i<counts.size(); ++i){
    h->Fill( double(i)+0.5, counts[i] );
    h->GetXaxis()->SetBinLabel(i+1,labels[i]);
  }
  h->SetMinimum(0.);
  h->GetXaxis()->SetLabelSize(0.06);

  h->Draw("H9");

  canvas->Update();
  canvas->Print("output/countGens3.pdf");

}
