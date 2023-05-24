void mlp(Int_t n_train = 50) {
   Int_t size;
   Double_t length = 0.;
   Double_t width = 0.;
   Double_t dist = 0.;
   Double_t azwidth = 0.;
   Double_t miss = 0.;
   Double_t alpha = 0.;

   TFile *filegamma = new TFile("../output/hgamma.root", "READ");
   TTree *hgammatree = (TTree*)filegamma->Get("hgamma");
   if (!hgammatree) {
      cerr << "Can't read gamma!" << endl;
      return;
   }


   TFile *fileproton = new TFile("../output/hproton.root", "READ");
   TTree *hprotontree = (TTree*)fileproton->Get("hproton");
   if (fileproton->IsZombie() || !hprotontree) {
      cerr << "Can't read proton!" << endl;
      return;
   }

   hgammatree->SetBranchAddress("size", &size);
   hgammatree->SetBranchAddress("length", &length);
   hgammatree->SetBranchAddress("width", &width);
   hgammatree->SetBranchAddress("dist", &dist);
   hgammatree->Branch("azwidth", &azwidth);
   hgammatree->Branch("miss", &miss);
   hgammatree->Branch("alpha", &alpha);

   hprotontree->SetBranchAddress("size", &size);
   hprotontree->SetBranchAddress("length", &length);
   hprotontree->SetBranchAddress("width", &width);
   hprotontree->SetBranchAddress("dist", &dist);
   hprotontree->Branch("azwidth", &azwidth);
   hprotontree->Branch("miss", &miss);
   hprotontree->Branch("alpha", &alpha);


   TFile *file = new TFile("empty.root", "RECREATE");
   TTree *tree = new TTree("tree", "tree");

   tree->Branch("size", &size, "size/I");
   tree->Branch("length", &length, "length/D");
   tree->Branch("width", &width, "width/D");
   tree->Branch("dist", &dist, "dist/D");
   tree->Branch("azwidth", &azwidth, "azwidth/D");
   tree->Branch("miss", &miss, "miss/D");
   tree->Branch("alpha", &alpha, "alpha/D");

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
   TMultiLayerPerceptron* mlp=new TMultiLayerPerceptron("@size,length,width,dist,azwidth,miss,alpha:7:7:type", tree,
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
   // type=1 gamma (signal), type=0 proton (background)
   mlpa->DrawNetwork(0,"type==1","type==0");

   cIO->cd(4);
   Int_t nbins = 500;
   Double_t leftboard = -0.5;
   Double_t rightboard = 1.5;
   TH1F *bg = new TH1F("bgh", "NN output", nbins, leftboard, rightboard);
   TH1F *sig = new TH1F("sigh", "NN output", nbins, leftboard, rightboard);
   bg->SetDirectory(0);
   sig->SetDirectory(0);
   Double_t params[7];
   for (Int_t i = 0; i < hprotontree->GetEntries(); i++) {
      hprotontree->GetEntry(i);
      params[0] = size;
      params[1] = length;
      params[2] = width;
      params[3] = dist;
      params[4] = azwidth;
      params[5] = miss;
      params[6] = alpha;
      bg->Fill(mlp->Evaluate(0, params));
   }
   for (Int_t i = 0; i < hgammatree->GetEntries(); i++) {
      hgammatree->GetEntry(i);
      params[0] = size;
      params[1] = length;
      params[2] = width;
      params[3] = dist;
      params[4] = azwidth;
      params[5] = miss;
      params[6] = alpha;
      sig->Fill(mlp->Evaluate(0,params));
   }
   bg->SetLineColor(kBlue);
   bg->SetFillStyle(3008);   bg->SetFillColor(kBlue);
   sig->SetLineColor(kRed);
   sig->SetFillStyle(3003); sig->SetFillColor(kRed);
   bg->SetStats(0);
   sig->SetStats(0);
   sig->Draw();
   bg->Draw("same");
   TLegend *legend = new TLegend(.75, .80, .95, .95);
   legend->AddEntry(bg, "Background (proton)");
   legend->AddEntry(sig, "Signal (gamma)");
   legend->Draw();
   cIO->cd(0);

   Double_t alp = 0.05;
   Double_t betta;

   Double_t sigI_full = sig->Integral(0, nbins, "width");
   Double_t bgI_full = bg->Integral(0, nbins, "width");
   cout << sigI_full << ", " << bgI_full << endl;

   Int_t nowbin = -1;
   Double_t test = alp + 1.;
   Double_t sigI, bgI;
   while (test > alp) {
      nowbin++;
      sigI = sig->Integral(nowbin, nbins, "width");
      bgI = bg->Integral(nowbin, nbins, "width");
      if ((sigI + bgI) == 0.)
         break;
      test = bgI / (sigI + bgI);
   }
   cout << "board is " << sig->GetBinCenter(nowbin) << endl;
   cout << "alp=" << alp << endl;
   cout << "betta=" << (sigI_full - sigI) / (sigI_full - sigI + bgI_full - bgI) << endl;
}
