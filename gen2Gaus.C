void gen2Gaus()
{
   Int_t n = 1000;
   Double_t E1 = 0.5;
   Double_t E2 = 0.25;
   Double_t D1 = 1.5;
   Double_t D2 = 0.25;

   ofstream out("data1.txt");

   TRandom rnd;

   for (Int_t i=0; i<n; i++) {
      for (Int_t j=0; j<6; j++) {
         out << rnd.Gaus(E1, E2) << " ";
      }
      out << -1 << endl;
   }

   out.close();

   out.open("data2.txt");

   for (Int_t i=0; i<n; i++) {
      for (Int_t j=0; j<6; j++) {
         out << rnd.Gaus(D1, D2) << " ";
      }
      out << 1 << endl;
   }

   out.close();


   fstream fin("data1.txt");
   if (!fin.is_open()) {
      cerr << "Can't open data1.txt!" << endl;
      return;
   }

   Int_t nBins = 100;
   TH1F* hE = new TH1F("hE", "hE", nBins, min(E1 - 5.*E2, D1 - 5.*D2), max(E1 + 5.*E2, D1 + 5.*D2));
   while (!fin.eof()) {
      Double_t num;
      fin >> num;
      if (num != -1 && num != 1) {
         hE->Fill(num);
      }
   }

   fin.close();

   //hE->Draw("SAME");

   fin.open("data2.txt");
   if (!fin.is_open()) {
      cerr << "Can't open data2.txt!" << endl;
      return;
   }

   TH1F* hD = new TH1F("hD", "hD", nBins, min(E1 - 5.*E2, D1 - 5.*D2), max(E1 + 5.*E2, D1 + 5.*D2));
   while (!fin.eof()) {
      Double_t num;
      fin >> num;
      if (num != -1 && num != 1) {
         hE->Fill(num);
      }
   }

   hE->Draw("SAME");

   fin.close();
}


