void draw()
{
  TString particle = "hgamma";
  TFile *file_gamma = new TFile("../output/" + particle + ".root");
  if (file_gamma->IsZombie()) {
    cerr << "Can't read input file_gamma!" << endl;
    return;
  }

  TTree *tree_gamma = (TTree*)file_gamma->Get(particle);
  if (!tree_gamma) {
    cerr << "Can't read tree_gamma!" << endl;
    return;
  }

  particle = "hproton";
  TFile *file_proton = new TFile("../output/" + particle + ".root");
  if (file_proton->IsZombie()) {
    cerr << "Can't read input file_proton!" << endl;
    return;
  }

  TTree *tree_proton = (TTree*)file_proton->Get(particle);
  if (!tree_proton) {
    cerr << "Can't read tree_proton!" << endl;
    return;
  }

  TCanvas* canv = new TCanvas("canv", "canv");
  TString param = "alpha";
  tree_gamma->Draw(param);
  TH1F* hist_gamma = (TH1F*)gPad->FindObject("htemp");
  hist_gamma->SetName("hist_gamma");
  hist_gamma->SetTitle(param);
  hist_gamma->SetLineColor(kRed);
  hist_gamma->SetFillStyle(3003);
  hist_gamma->GetYaxis()->SetTitle("count");
  Int_t nbins = hist_gamma->GetNbinsX();
  TH1F* hist_proton = new TH1F("gamma", "gamma", nbins, hist_gamma->GetBinCenter(0), hist_gamma->GetBinCenter(nbins));
  tree_proton->Draw(param+">>hist_proton", "", "SAME");
  gPad->Update();
  gPad->SetLogy();
  hist_gamma->SetStats(0);

  TLegend *legend = new TLegend(.75, .80, .95, .95);
  legend->AddEntry(hist_proton, "proton");
  legend->AddEntry(hist_gamma, "gamma");
  legend->Draw();

  canv->SaveAs("output/"+param+".png");
}
