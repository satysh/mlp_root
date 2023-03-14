void makeTree()
{
   fstream finE("data1.txt");
   if (!finE.is_open()) {
      cerr << "Can't open data1.txt!" << endl;
      return;
   }
   fstream finD("data2.txt");
   if (!finD.is_open()) {
      cerr << "Can't open data2.txt!" << endl;
      return;
   }

   TTree* mixedTree = new TTree("tree", "tree");
   Double_t x0, x1, x2, x3, x4, x5, x6;
   mixedTree->Branch("x0", &x0);
   mixedTree->Branch("x1", &x1);
   mixedTree->Branch("x2", &x2);
   mixedTree->Branch("x3", &x3);
   mixedTree->Branch("x4", &x4);
   mixedTree->Branch("x5", &x5);
   mixedTree->Branch("x6", &x6);

   TRandom rnd;
   Int_t n = 0;
   while (n < 1000) {
      if (rnd.Uniform() <= 0.5) {
         finE >> x0 >> x1 >> x2 >> x3 >> x4 >> x5 >> x6;
         mixedTree->Fill();
      }
      else {
         finD >> x0 >> x1 >> x2 >> x3 >> x4 >> x5 >> x6;
         mixedTree->Fill();
      }

      if (finE.eof()) {
         while (!finD.eof()) {
            finD >> x0 >> x1 >> x2 >> x3 >> x4 >> x5 >> x6;
            mixedTree->Fill();
            break;
         }
         break;
      }
      else if (finD.eof()) {
         while (!finE.eof()) {
            finE >> x0 >> x1 >> x2 >> x3 >> x4 >> x5 >> x6;
            mixedTree->Fill();
            break;
         }
         break;
      }
   }

   TFile* file = new TFile("mixeddata.root", "RECREATE");
   mixedTree->Write();
}
