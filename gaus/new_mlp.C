void new_mlp() {
   
   // 0.5, 0.25, 1.5, 0.25
   // 1., 0.25, 1.5, 0.25
   // 1.3, 0.25, 1.5, 0.25
   // 1.3, 0.25, 1.5, 1.25
   Int_t n_train = 200;
   Int_t n = 800;
   Int_t test_n = 100;
   Double_t E1 = 1.3;
   Double_t E2 = 0.25;
   Double_t D1 = 1.5;
   Double_t D2 = 1.25;

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
      x6 = -1;
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
   TMultiLayerPerceptron* mlp=new TMultiLayerPerceptron("x0,x1,x2,x3,x4,x5:5:5:type", tree,
                                                        "Entry$%2","(Entry$+1)%2"/*, TNeuron::kTanh*/);

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
   
   TTree* test_tree = new TTree("test_tree", "test_tree");
   test_tree->Branch("x0", &x0, "x0/D");
   test_tree->Branch("x1", &x1, "x1/D");
   test_tree->Branch("x2", &x2, "x2/D");
   test_tree->Branch("x3", &x3, "x3/D");
   test_tree->Branch("x4", &x4, "x4/D");
   test_tree->Branch("x5", &x5, "x5/D");
   
   for (Int_t i=0; i<test_n; i++) {
      x0 = rnd.Gaus(E1, E2);
      x1 = rnd.Gaus(E1, E2);
      x2 = rnd.Gaus(E1, E2);
      x3 = rnd.Gaus(E1, E2);
      x4 = rnd.Gaus(E1, E2);
      x5 = rnd.Gaus(E1, E2);
      test_tree->Fill();
   }

   for (Int_t i=0; i<test_n; i++) {
      x0 = rnd.Gaus(D1, D2);
      x1 = rnd.Gaus(D1, D2);
      x2 = rnd.Gaus(D1, D2);
      x3 = rnd.Gaus(D1, D2);
      x4 = rnd.Gaus(D1, D2);
      x5 = rnd.Gaus(D1, D2);
      test_tree->Fill();
   }

   Int_t BinN = 100;
   TH1F* base = new TH1F("base", "", BinN, min(E1 - 5.*E2, D1 - 5.*D2), max(E1 + 5.*E2, D1 + 5.*D2));
   TH1F* si = new TH1F("si", "si", BinN, min(E1 - 5.*E2, D1 - 5.*D2), max(E1 + 5.*E2, D1 + 5.*D2));
   TH1F* bg = new TH1F("bg", "bg", BinN, min(E1 - 5.*E2, D1 - 5.*D2), max(E1 + 5.*E2, D1 + 5.*D2));

   Double_t params[6];
   for (Int_t i=0; i<test_tree->GetEntries(); i++) {
      test_tree->GetEntry(i);
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
   
   TCanvas* resultCanv = new TCanvas("result", "result");
   resultCanv->cd();
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
   legend->AddEntry(base, "input");
   legend->AddEntry(bg, "background");
   legend->AddEntry(si, "signal");
   //legend->SetTextSize(20);
   legend->Draw();
   resultCanv->cd(0);
    
   TString text;
   /*text.Form("background mean is %f, stdDev is %f vs. result mean is %f, result stdDev is %f \n
              signal mean is %f, stdDev is %f vs. result mean is %f, stdDev is %f",
              E1, E2, bg->GetMean(), bg->GetStdDev(), D1, D2, 0., 0.);*/
   text.Form("bg mean is %f, stdDev is %f result mean is %f, stdDev is %f"
             , D1, D2, bg->GetMean(), bg->GetStdDev());
   TText *t = new TText(.5, .5, text.Data());
   text.Form("sig mean is %f, stdDev is %f result mean is %f, stdDev is %f"
             , E1, E2, si->GetMean(), si->GetStdDev());
   t->SetTextAlign(22);
   t->SetTextColor(kRed+2);
   t->SetTextFont(23);
   t->SetTextSize(15);
   cIO->cd(4);
   t->Draw();
   t->DrawText(.5, .4, text);
   cIO->cd(0);
}
