void new_mlp() {

   Int_t n = 1000;
   Double_t E1 = 1.;
   Double_t E2 = 0.25;
   Double_t D1 = 1.5;
   Double_t D2 = 0.25;

   TRandom rnd;

   TTree* tree = new TTree("data", "data");
   Double_t x0, x1, x2, x3, x4, x5;
   Int_t x6;
   tree->Branch("x0", &x0, "x0/D");
   tree->Branch("x1", &x1, "x1/D");
   tree->Branch("x2", &x2, "x2/D");
   tree->Branch("x3", &x3, "x3/D");
   tree->Branch("x4", &x4, "x4/D");
   tree->Branch("x5", &x5, "x5/D");

   Int_t type;
   tree->Branch("type", &type, "type/I");
   type = 1;
   for (Int_t i=0; i<n; i++) {
      x0 = rnd.Gaus(E1, E2);
      x1 = rnd.Gaus(E1, E2);
      x2 = rnd.Gaus(E1, E2);
      x3 = rnd.Gaus(E1, E2);
      x4 = rnd.Gaus(E1, E2);
      x5 = rnd.Gaus(E1, E2);
      x6 = 0;
      tree->Fill();
   }

   type = 0;
   for (Int_t i=0; i<n; i++) {
      x0 = rnd.Gaus(D1, D2);
      x1 = rnd.Gaus(D1, D2);
      x2 = rnd.Gaus(D1, D2);
      x3 = rnd.Gaus(D1, D2);
      x4 = rnd.Gaus(D1, D2);
      x5 = rnd.Gaus(D1, D2);
      x6 = 1;
      tree->Fill();
   }


   // create ANN
   // kTanh    TMultiLayerPerceptron (const char *layout, TTree *data, TEventList *training, TEventList *test,
   //TNeuron::ENeuronType type=TNeuron::kSigmoid, const char *extF="", const char *extD="")
   TMultiLayerPerceptron* mlp=new TMultiLayerPerceptron("x0,x1,x2,x3,x4,x5:6:type", tree,
                                                        "Entry$%2","(Entry$+1)%2"/*, TNeuron::kTanh*/);

   mlp->Train(200,"text graph update=10");
   //tree->StartViewer();
   //mlp->Export("test","python");
   // analyze it
   TCanvas* cIO=new TCanvas("Info", "Info");
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

   Int_t BinN = 100;
   TH1F* base = new TH1F("base", "base", BinN, min(E1 - 5.*E2, D1 - 5.*D2), max(E1 + 5.*E2, D1 + 5.*D2));
   TH1F* si = new TH1F("si", "si", BinN, min(E1 - 5.*E2, D1 - 5.*D2), max(E1 + 5.*E2, D1 + 5.*D2));
   TH1F* bg = new TH1F("bg", "bg", BinN, min(E1 - 5.*E2, D1 - 5.*D2), max(E1 + 5.*E2, D1 + 5.*D2));

   Double_t params[6];
   for (Int_t i=0; i<tree->GetEntries(); i++) {
      tree->GetEntry(i);
      params[0] = x0;
      params[1] = x1;
      params[2] = x2;
      params[3] = x3;
      params[4] = x4;
      params[5] = x5;
      for (Int_t j=0; j<6; j++) {
         base->Fill(params[j]);
      }
      Double_t idnt = mlp->Evaluate(0,params);
      if (idnt > 0.5) {
         for (Int_t j=0; j<6; j++) {
            si->Fill(params[j]);
         }
      }
      else {
         for (Int_t j=0; j<6; j++) {
            bg->Fill(params[j]);
         }
      }
   }

   cIO->cd(4);
   base->Draw();
   base->SetLineColor(kGreen);
   base->SetFillStyle(3003); base->SetFillColor(kGreen);
   base->SetLineWidth(2);
   si->Draw("SAME");
   si->SetLineColor(kRed);
   //si->SetFillStyle(3003); si->SetFillColor(kRed);
   bg->Draw("SAME");
   //bg->SetFillStyle(3008); bg->SetFillColor(kBlue);
   bg->SetLineColor(kBlue);
   base->SetStats(0);
   si->SetStats(0);
   bg->SetStats(0);

   TLegend *legend = new TLegend(.75, .80, .95, .95);
   TString bgTitle;
   bgTitle.Form("mean %f, sigma %f", E1, E2);
   TString siTitle;
   siTitle.Form("mean %f, sigma %f", D1, D2);
   legend->AddEntry(bg, bgTitle);
   legend->AddEntry(si, siTitle);
   //legend->SetTextSize(20);
   legend->Draw();
   cIO->cd(0);
}
