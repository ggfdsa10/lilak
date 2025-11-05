
Double_t fPulseData[300];

Double_t pulseFunc(Double_t* x, Double_t* par)
{
    int tbIdx = x[0] - (par[0] - 100);

    double adc = fabs(par[1]);
    if(tbIdx < 0){return par[2]+adc*fPulseData[0];}
    else if(tbIdx >= 300){return par[2]+adc*fPulseData[299];}
    return par[2]+adc*fPulseData[tbIdx];
}


Double_t multiPulse(Double_t* x, Double_t* par)
{
    int pulseNum = par[0];
    Double_t pedestal = par[1];

    Double_t sumFit = 0.;
    for(int i=0; i<pulseNum; i++){
        int parIdx = 2+i*2;
        int tbIdx = x[0] - (par[parIdx] - 100.);
        double adc = par[parIdx+1];

        if(tbIdx < 0 || tbIdx >= 300){continue;}
        sumFit += adc*fPulseData[tbIdx];
    }

    return sumFit+pedestal;
}

void testPulse()
{
    TFile* file = new TFile("/home/shlee/workspace/lilak/tpc-Drum/common/Pulse_template_502PT_20ns.root", "read");
    TTree* tree = (TTree*)file -> Get("pulse");

    double pulse[300];
    tree -> SetBranchAddress("template", &pulse);
    tree -> GetEntry(0);

    for(int tb=0; tb<300; tb++){
        double adc = pulse[tb];
        if(tb > 100.){adc = adc - pulse[200];}
        fPulseData[tb] = adc;
        if(tb < 50. || tb > 200.){fPulseData[tb] = 0.;}
    }

    TFile* noiseFile = new TFile("/home/shlee/workspace/lilak/tpc-Drum/common/Noise_template_test.root", "read");
    TTree* NoiseTree = (TTree*)noiseFile -> Get("event");

    Double_t noiseData[512];
    NoiseTree -> SetBranchAddress("Noise", &noiseData);
    int noiseEventNum = NoiseTree -> GetEntries();

    // ================================================================================
    const int config1 = 1;
    const int eventNum = 768*10;
    const int generatePulseNum = 3;
    bool drawingOn = true;

    TH1D* hTB = new TH1D("hTB", "", 512, 0, 512);
    hTB -> SetStats(0);
    hTB -> SetLineWidth(2);
    hTB -> SetLineColor(kBlack);

    TH1D* hTruthPulse[generatePulseNum];
    for(int n=0; n<generatePulseNum; n++){
        hTruthPulse[n] = new TH1D(Form("%i_pulse", n), "", 512, 0, 512);
    }
    TGraph* specPoint = new TGraph();
    specPoint -> SetMarkerStyle(20);
    specPoint -> SetMarkerSize(1);
    specPoint -> SetMarkerColor(kRed);

    TLatex* latex = new TLatex();

    TH1D* hChi2 = new TH1D("", "", 40, 0., 10.);


    // ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2","Migrad");
    // ROOT::Math::MinimizerOptions::SetDefaultErrorDef(0.5);      // NLL
    ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
    // ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(200000);
    ROOT::Math::MinimizerOptions::SetDefaultTolerance(1e-7);
    // TVirtualFitter::SetMaxIterations(20000);
    ROOT::Math::IntegratorOneDimOptions::SetDefaultIntegrator("GaussLegendre");
    ROOT::Math::IntegratorOneDimOptions::SetDefaultNPoints(6);
    // ROOT::Math::IntegratorOneDimOptions::SetDefaultRelTolerance(1e-10);


    TCanvas* c1 = new TCanvas("ccanvase","", 600., 600.);

    double genNoise[512];

    for(int c = 0; c<config1; c++){
        int tbTruth = 100;
        int tbDiff = gRandom->Gaus(0., 30.);

        // Generate Multiple pulse events
        for(int event=0; event<eventNum; event++){
            cout << " event: " << event<< endl;

            hTB -> Reset("ICESM");

            gRandom -> SetSeed(0);

            // ================= Pulse generation ======================
            double pedestalLevel = gRandom->Uniform(460., 540.);
            memset(genNoise, 0., sizeof(genNoise));
            int mixNum = gRandom->Uniform(2, 5);
            for(int i=0; i<mixNum; i++){
                double sign = (gRandom -> Uniform(0, 2) == 0)? -1. : 1;
                NoiseTree -> GetEntry(gRandom->Uniform(0, noiseEventNum));
                for(int tb=0; tb<512; tb++){
                    genNoise[tb] += (sign*noiseData[tb]);
                }
            }
            for(int n=0; n<generatePulseNum; n++){
                hTruthPulse[n] -> Reset("ICESM");
                int tbDiff = gRandom->Gaus(5., 40.);
                int adcTruth = gRandom->Uniform(30., 70.);

                for(int i=0; i<300; i++){
                    double tb = (i-100) + tbTruth+tbDiff*n;
                    double adc = fPulseData[i] * adcTruth;
                    if(tb < 0 || tb > 511){continue;}

                    double entry = hTB -> GetBinContent(tb+1) + adc + pedestalLevel;
                    // if(entry > 4096){entry = 4096;}
                    hTB -> SetBinContent(tb+1, entry - pedestalLevel);
                    hTruthPulse[n] -> SetBinContent(tb+1, adc);
                }
            }

            double NoiseScale = 0.7e4*gRandom->Gaus(1., 1.5);
            for(int tb=0; tb<512; tb++){
                double entry = hTB -> GetBinContent(tb+1) + genNoise[tb]*NoiseScale;
                // double entry = genNoise[tb]*NoiseScale;
                // if(entry < 40.){entry = 0.;}
                hTB -> SetBinContent(tb+1, entry);
            }

            // ====================================================================================
            // Pulse seperation method !!!

            TSpectrum* spectrum = new TSpectrum();
            double threshold_spec = 0.008; // peak must be over Threshold_spec * ADC of highest peak 
            double sigma_spec = 1.; // ?? 

            spectrum->Search(hTB, sigma_spec, "nodraw goff", threshold_spec);
            int numPeak = spectrum->GetNPeaks();
            auto tSpecPeakX = spectrum->GetPositionX();
            auto tSpecPeakY = spectrum->GetPositionY();

            int peakNum = 0;
            for(int peak=0; peak<numPeak; peak++){
                double tb = tSpecPeakX[peak];
                double adc = tSpecPeakY[peak];
                if(adc < 20.){continue;}
                peakNum++;
            }

            TF1* fit = new TF1(Form("pulseFit_%i", 1), &multiPulse, 10., 490., 2*peakNum+2);
            fit -> SetLineColor(kGreen+2);
            fit -> FixParameter(0, peakNum);
            fit -> FixParameter(1, 0.);

            specPoint -> Set(0);
            for(int peak=0; peak<peakNum; peak++){
                double tb = tSpecPeakX[peak];
                double adc = tSpecPeakY[peak];
                if(adc < 20.){continue;}
                specPoint -> SetPoint(specPoint->GetN(), tb, adc);
                
                fit -> SetParameter(2+peak*2, tb);
                fit -> SetParameter(2+peak*2+1, adc);

                cout << "peak: " << peak << " | " << tb << " " << adc << endl;
            }
            hTB -> Fit(fit, "RQIM");

            // hChi2 -> Fill(fit->GetChisquare()/double(fit -> GetNDF()));

            if(drawingOn){
                c1 -> cd(1);
                // gPad -> SetLogy();
                hTB -> GetYaxis()->SetRangeUser(-50., 200.);
                hTB -> Draw();
                for(int n=0; n<generatePulseNum; n++){
                    hTruthPulse[n] -> Draw("same");
                }
                specPoint -> Draw("same, p");
                fit -> Draw("same");

                TLegend* leg = new TLegend(0.5, 0.65,0.89, 0.89);
                leg -> AddEntry(hTB, "Gen. Pulse");
                leg -> AddEntry(specPoint, "Esti. Peaks", "p");
                leg -> AddEntry(fit, "all Fit");

                for(int i=0; i<peakNum; i++){
                    TF1* recoPulse = new TF1(Form("recoPulse_%i", i), &pulseFunc, 10., 490., 3);
                    recoPulse -> SetParameter(2, fit->GetParameter(1));
                    recoPulse -> SetParameter(0, fit->GetParameter(2+i*2));
                    recoPulse -> SetParameter(1, fit->GetParameter(2+i*2+1));
                    recoPulse -> Draw("same");

                    if(i==0){
                        leg -> AddEntry(recoPulse, "Pulse Fit");
                    }
                }
                latex -> DrawLatexNDC(0.12, 0.85, "Pulse ToyMC");
                latex -> DrawLatexNDC(0.12, 0.79, Form("Esti. #Peak = %i", peakNum));
                leg -> Draw("same");
                c1 -> Update();
                c1 -> SaveAs(Form("./pulse/pulse_event%i.png", event));
            }
        }
    }
}