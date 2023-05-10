const double rad_to_deg = 180./3.1415926535;
const double s_pmt = 4; // 23.4 [mm]
void tree_form(TString inBinFileName = "", TString outFilePath = "")
{
  if (inBinFileName == "") {
    cerr << "You have to define input path/name for binary file!" << endl;
    return;
  }
  else if (outFilePath == "") {
    cerr << "You have to define path for ouput file!" << endl;
  }

  TFile* file = new TFile(outFilePath+"/data.root", "RECREATE");
  /* Binary file header */
  int32_t N_run;
  int32_t i_scattering;
  int32_t i_tel;
  int32_t N_b_f;

  double E;
  double theta;
  double phi;
  double X_core[3] = {};
  double h1int;
  double Particle_type;
  double xmax;
  double hmax;
  double X_telescope[3] = {};
  double X_offset[2] = {};

  double theta_tel;
  double phi_tel;
  double da1;
  double da0;
  double T0;

  int32_t N_pixels;
  int32_t photoSum;

  /* Addition params which is being calculated*/
  double x_tel_afterOffset;
  double y_tel_afterOffset;
  double distance;

  ifstream fin(inBinFileName.Data(), ios::binary);
  if (!fin.is_open()) {
    cerr << "Input file read error!" << endl;
    return;
  }
  else {
    cout << "\n### Input file was read successful! ###\n" << endl;
  }

  TTree* tree = new TTree("tree", "data");
  tree->Branch("N_run", &N_run, "N_run/I");
  tree->Branch("i_scattering", &i_scattering, "i_scattering/I");
  tree->Branch("i_tel", &i_tel, "i_tel/I");
  tree->Branch("N_b_f", &N_b_f, "N_b_f/I");
  tree->Branch("Energy", &E, "Energy/D");
  tree->Branch("theta", &theta, "theta/D");
  tree->Branch("phi", &phi, "phi/D");
  tree->Branch("X_core_x", &X_core[0], "X_core_x/D");
  tree->Branch("X_core_y", &X_core[1], "X_core_y/D");
  tree->Branch("X_core_z", &X_core[2], "X_core_z/D");
  tree->Branch("h1int", &h1int, "h1int/D");
  tree->Branch("Particle_type", &Particle_type, "Particle_type/D");
  tree->Branch("xmax", &xmax, "xmax/D");
  tree->Branch("hmax", &hmax, "hmax/D");
  tree->Branch("X_telescope_x", &X_telescope[0], "X_telescope_x/D");
  tree->Branch("X_telescope_y", &X_telescope[1], "X_telescope_y/D");
  tree->Branch("X_telescope_z", &X_telescope[2], "X_telescope_z/D");
  tree->Branch("X_offset_x", &X_offset[0], "X_offset_x/D");
  tree->Branch("X_offset_y", &X_offset[1], "X_offset_y/D");
  tree->Branch("theta_tel", &theta_tel, "theta_tel/D");
  tree->Branch("phi_tel", &phi_tel, "phi_tel/D");
  tree->Branch("da1", &da1, "da1/D");
  tree->Branch("da0", &da0, "da0/D");
  tree->Branch("T0", &T0, "T0/D");
  tree->Branch("N_pixels", &N_pixels, "N_pixels/I");
  tree->Branch("x_tel_afterOffset", &x_tel_afterOffset, "x_tel_afterOffset/D");
  tree->Branch("y_tel_afterOffset", &y_tel_afterOffset, "y_tel_afterOffset/D");
  tree->Branch("photoSum", &photoSum, "photoSum/I");
  tree->Branch("distance", &distance, "distance/D");

  int32_t amplitude;
  int32_t N_r;
  int32_t N_c;
  double x_pmt;
  double y_pmt;
  TTree* pixels_tree = new TTree("pixels_tree", "pixels_tree");
  pixels_tree->Branch("amplitude", &amplitude, "amplitude/I");
  pixels_tree->Branch("N_r", &N_r, "N_r/I");
  pixels_tree->Branch("N_c", &N_c, "N_c/I");
  pixels_tree->Branch("x_pmt", &x_pmt, "x_pmt/D");
  pixels_tree->Branch("y_pmt", &y_pmt, "y_pmt/D");

  int debug = 0;
  int debug_level = 0;
  int pixel_num_min = 4000;
  int pixel_num_max = -4000;
  int N_r_min = 4000;
  int N_r_max = -4000;
  int N_c_min = 4000;
  int N_c_max = -4000;
  while (!fin.eof()) {
    fin.read((char*)&N_run, sizeof(int32_t));
    fin.read((char*)&i_scattering, sizeof(int32_t));
    fin.read((char*)&i_tel, sizeof(int32_t));
    fin.read((char*)&N_b_f, sizeof(int32_t));
    fin.read((char*)&E, sizeof(double));
    fin.read((char*)&theta, sizeof(double));
    fin.read((char*)&phi, sizeof(double));
    fin.read((char*)X_core, sizeof(X_core));
    fin.read((char*)&h1int, sizeof(double));
    fin.read((char*)&Particle_type, sizeof(double));
    fin.read((char*)&xmax, sizeof(double));
    fin.read((char*)&hmax, sizeof(double));
    fin.read((char*)X_telescope, sizeof(X_telescope));
    fin.read((char*)X_offset, sizeof(X_offset));
    fin.read((char*)&theta_tel, sizeof(double));
    fin.read((char*)&phi_tel, sizeof(double));
    fin.read((char*)&da1, sizeof(double));
    fin.read((char*)&da0, sizeof(double));
    fin.read((char*)&T0, sizeof(double));
    fin.read((char*)&N_pixels, sizeof(int32_t));

    /*Conversion of values dimensions*/
    theta *= rad_to_deg;
    phi *= rad_to_deg;
    theta_tel *= rad_to_deg;
    phi_tel *= rad_to_deg;
    E *= 1e-9; // eV to GeV
    h1int *= 1e-6; // mm to km
    da1 *= rad_to_deg;
    da0 *= rad_to_deg;
    //T0 *= 1e+3; // sec to mcsec
    for (int i = 0; i < 3; i++) {
      X_telescope[i] *= 1e-3;
      if (i < 2)
        X_offset[i] *= 1e-3;
    }

    Double_t x0 = -1.*X_offset[0]; // x
    Double_t y0 = -1.*X_offset[1]; // y
    Double_t x1 = X_telescope[0];
    Double_t y1 = X_telescope[1];
    Double_t z1 = X_telescope[2];
    distance = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + z1*z1 );
    /* Calculations */
    x_tel_afterOffset = X_telescope[0] + X_offset[0];
    y_tel_afterOffset = X_telescope[1] + X_offset[1];

    photoSum = 0;
    for (int i = 0; i < N_pixels; i++) {
      double mean_t;
      double rms_t;
      fin.read((char*)&amplitude, sizeof(int32_t));
      fin.read((char*)&N_r, sizeof(int32_t));
      fin.read((char*)&N_c, sizeof(int32_t));
      fin.read((char*)&mean_t, sizeof(double));
      fin.read((char*)&rms_t, sizeof(double));

      /*photoSum += cur_ampl; // sum apmlitudes

      pixel_num = (int)cur_pix_num;
      amplitude = (int)cur_ampl;*/
      /* Calculations */
      /*
      for (int k = -19; k < 20; k++) {
        for (int kk = -19; kk < 20; kk++) {
          if ((k*256 + kk + 128) == pixel_num) {
            N_r = k; N_c = kk;
            k = 256; break;
          }
          if (k == 127 && kk == 127) {
            cout << "For pixel_num " << pixel_num << " was found nothing!" << endl;
            k = 256; break;
          }
        }
      }*/

      /* Calculations */
      x_pmt = s_pmt / 2. * (2.*(Double_t)N_c - (Double_t)(N_r&1));
      y_pmt = (Double_t)N_r*s_pmt*TMath::Sin(60.*TMath::DegToRad());

      /*if (pixel_num_min > pixel_num)
      	pixel_num_min = pixel_num;
      if (pixel_num_max < pixel_num)
      	pixel_num_max = pixel_num;
      //if (pixel_num == 0)
      //	cout << "pixel_num is zero" << endl;
      if (N_r_min > N_r)
      	N_r_min = N_r;
      if (N_r_max < N_r)
      	N_r_max = N_r;
      if (N_c_min > N_c)
      	N_c_min = N_c;
      if (N_c_max < N_c)
      	N_c_max = N_c;*/

      pixels_tree->Fill();

      /* Cheking */
    }

    /* Fill main tree by current values*/
    tree->Fill();

    debug++;
    //if (debug == 31127) break;
    if (debug_level <= 100*debug/31128) {
      cout << "[" << setw(3) << 100*debug/31128 << "] was completed!" << endl;
      debug_level +=5;
    }
  }
  fin.clear();
  fin.close();
  tree->Write();
  pixels_tree->Write();
  file->Close();
  cout << "The tree was written in the file data.root successfully!" << endl;
/*
  cout << "INFO: " << endl;
  cout << "Minimal pixel_num is " << pixel_num_min << endl;
  cout << "Maximum pixel_num is " << pixel_num_max << endl;
  cout << "N_r minimum is " << N_r_min << ", and maximum is " << N_r_max << endl;
  cout << "N_c minimum is " << N_c_min << ", and maximum is " << N_c_max << endl;
*/
}
