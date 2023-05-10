void hillas()
{
  TString particle = "gamma";
  TFile *file = new TFile("../output/" + particle + ".root");
  if (file->IsZombie()) {
    cerr << "Can't read input file!" << endl;
    return;
  }

  TTree *tree = (TTree*)file->Get("tree");
  if (!tree) {
    cerr << "Can't read tree!" << endl;
    return;
  }

  TTree* pixels_tree = (TTree*)file->Get("pixels_tree");
  if (!pixels_tree) {
    cerr << "Can't read input tree!" << endl;
    return;
  }

  int32_t N_pixels;
  int32_t n_buff;
  int32_t amplitude;
  double x_pmt;
  double y_pmt;
  tree->SetBranchAddress("N_pixels", &N_pixels);
  tree->SetBranchAddress("N_b_f", &n_buff);
  pixels_tree->SetBranchAddress("amplitude", &amplitude);
  pixels_tree->SetBranchAddress("x_pmt", &x_pmt);
  pixels_tree->SetBranchAddress("y_pmt", &y_pmt);

  TFile* outFile = new TFile("../output/h" + particle + ".root", "RECREATE");
  TTree* outTree = new TTree("h" + particle, "h" + particle);
  Int_t size;
  Double_t length = 0.;
  Double_t width = 0.;
  Double_t dist = 0.;
  Double_t azwidth = 0.;
  Double_t miss = 0.;
  Double_t alpha = 0.;

  outTree->Branch("size", &size, "size/I");
  outTree->Branch("length", &length, "length/D");
  outTree->Branch("width", &width, "width/D");
  outTree->Branch("dist", &dist, "dist/D");
  outTree->Branch("azwidth", &azwidth, "azwidth/D");
  outTree->Branch("miss", &miss, "miss/D");
  outTree->Branch("alpha", &alpha, "alpha/D");

  Int_t start = 0;
  for (Int_t i=0; i<tree->GetEntries(); i++) {
    tree->GetEntry(i);
    size = n_buff;
    Double_t curXNo = 0.;
    Double_t curYNo = 0.;
    Double_t curX2No = 0.;
    Double_t curY2No = 0.;
    Double_t curXYNo = 0.;
    Int_t sum = 0;
    for (Int_t j=start; j<(N_pixels + start); j++) {
      pixels_tree->GetEntry(j);
      sum += amplitude;
      Double_t curWeight = (Double_t)amplitude/n_buff;
      curXNo += curWeight*(Double_t)x_pmt;
      curYNo += curWeight*(Double_t)y_pmt;
      curX2No += curWeight*(Double_t)x_pmt*x_pmt;
      curY2No += curWeight*(Double_t)y_pmt*y_pmt;
      curXYNo += curWeight*x_pmt*y_pmt;
    }
    start += N_pixels;
    if (sum < 10)
      continue;

    /* Calculations */
    Double_t cxx = curX2No - curXNo*curXNo;
    Double_t cyy = curY2No - curYNo*curYNo;
    Double_t cxy = curXYNo - curXNo*curYNo;
    Double_t checker = curXNo*curXNo + curYNo*curYNo;
    Double_t d = cyy - cxx;
    Double_t z = TMath::Sqrt(d*d + 4.*cxy*cxy);
    Double_t u = 1. + d/z;
    Double_t v = 2. - u;
    miss = 0.5*(u*curXNo*curXNo + v*curYNo*curYNo) - 2.*cxy*curXNo*curYNo/z;
    alpha = asin(miss/dist)*TMath::RadToDeg();

    dist = TMath::Sqrt(checker);
    //cout << dist << endl;
    checker = curXNo*curXNo*curY2No - 2.*curXNo*curYNo*curXYNo + curX2No*curYNo*curYNo;
    if (dist != 0. and checker > 0) {
      azwidth = TMath::Sqrt(checker / dist / dist);
    }

    Double_t aCof = curYNo / curXNo;
    checker = (cxx + 2.*aCof*cxy + aCof*aCof*cyy) / (1. + aCof*aCof);
    if (checker < 0) {
        cerr << "WARNING!!! sqrt(< 0)!!!" << endl;
        cout << "Event: " << i << endl;
    }
    else {
      length = TMath::Sqrt(checker);
    }
    checker = (aCof*aCof*cxx - 2.*aCof*cxy + cyy) / (1. + aCof*aCof);
    if (checker < 0) {
      cerr << "WARNING!!! sqrt(< 0)!!!" << endl;
      cout << "Event: " << i << endl;
    }
    else {
      width = TMath::Sqrt(checker);
    }
    outTree->Fill();
  }

  outTree->Write();
}
