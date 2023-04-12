void hillas()
{
  TFile *file = new TFile("../output/gamma.root");
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

  TFile* outFile = new TFile("../output/hgamma.root", "RECREATE");
  TTree* outTree = new TTree("hgamma", "hgamma");
  Int_t size;
  Double_t length = 0.;
  Double_t width = 0.;
  Double_t dist = 0.;

  outTree->Branch("size", &size, "size/I");
  outTree->Branch("length", &length, "length/D");
  outTree->Branch("width", &width, "width/D");
  outTree->Branch("dist", &dist, "dist/D");

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

    /* Calculations */
    Double_t cxx = curX2No - curXNo*curXNo;
    Double_t cyy = curY2No - curYNo*curYNo;
    Double_t cxy = curXYNo - curXNo*curYNo;
    Double_t checker = curXNo*curXNo + curYNo*curYNo;
    if (checker < 0) {
      cerr << "WARNING!!! sqrt(< 0)!!!" << endl;
      cout << "Event: " << i << endl;
    }
    else {
      dist = TMath::Sqrt(checker);
      //cout << dist << endl;
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
