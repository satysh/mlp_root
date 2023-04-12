void mlp(Int_t n_train = 100) {
   Int_t size;
   Double_t length = 0.;
   Double_t width = 0.;
   Double_t dist = 0.;

   TFile *filegamma = new TFile("../output/hgamma.root", "READ");
   TTree *hgammatree = (TTree*)filegamma->Get("hgamma");
   if (!hgammatree) {
      cerr << "Can't read gamma!" << endl;
      return;
   }


   TFile *fileproton = new TFile("../output/hproton.root", "READ");
   TTree *hprotontree = (TTree*)fileproton->Get("h");
   if (fileproton->IsZombie() || !hprotontree) {
      cerr << "Can't read gamma!" << endl;
      return;
   }

   hgammatree->SetBranchAddress("size", &size);
   hgammatree->SetBranchAddress("length", &length);
   hgammatree->SetBranchAddress("width", &width);
   hgammatree->SetBranchAddress("dist", &dist);

   hprotontree->SetBranchAddress("size", &size);
   hprotontree->SetBranchAddress("length", &length);
   hprotontree->SetBranchAddress("width", &width);
   hprotontree->SetBranchAddress("dist", &dist);


   TFile *file = new TFile("empty.root", "RECREATE");
   TTree *tree = new TTree("tree", "tree");

   tree->Branch("size", &size, "size/I");
   tree->Branch("length", &length, "length/D");
   tree->Branch("width", &width, "width/D");
   tree->Branch("dist", &dist, "dist/D");

   Int_t type;
   tree->Branch("type", &type, "type/I");

   type = 1;
   for (Int_t i=0; i<(Int_t)(hgammatree->GetEntries()*0.8); i++) {
      hgammatree->GetEntry(i);
      tree->Fill();
   }

   type = 0;
   for(Int_t i=0; i<(Int_t)(hprotontree->GetEntries()*0.8); i++) {
      hprotontree->GetEntry(i);
      tree->Fill();
   }

   // create ANN
   TMultiLayerPerceptron* mlp=new TMultiLayerPerceptron("size,length,width,dist:5:5:type", tree,
                                                        "Entry$%2","(Entry$+1)%2");
   mlp->Train(n_train,"text graph update=10");
   //tree->StartViewer();
   //mlp->Export("test","python");
   // analyze it

   TCanvas* cIO=new TCanvas("Info", "NN Info");
   cIO->Divide(2,2);
   cIO->cd(1);
   TMLPAnalyzer* mlpa = new TMLPAnalyzer(mlp);
   // Initialisation
   mlpa->GatherInformations();
   // output to the console
   mlpa->CheckNetwork();
   // shows how each variable influences the network
   mlpa->DrawDInputs();

   cIO->cd(2);
   // shows the network structure
   mlp->Draw();

   cIO->cd(3);
   // draws the resulting network
   mlpa->DrawNetwork(0,"type==1","type==0");
}
