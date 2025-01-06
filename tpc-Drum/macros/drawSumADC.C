void drawSumADC()
{
    const int fileNum = 6;
    int FileRunName[fileNum] = {240822001, 240823002, 240823003, 240823004, 240824001, 240824002};
    // int FileRunName[fileNum] = {240822001, 240823002, 240823003, 241014001, 240824001, 240824002, 240824003};
    int FileAsAdidx[fileNum] = {0, 1, 2, 0, 1, 2};
    int FileVoltage[fileNum] = {2000, 2000, 2000, 2100, 2100, 2100};

    const int voltageNum = 2;
    const int asadNum = 3;
    const int sectionNum = 4;

    TFile* InFile[fileNum];
    TH1D* hist[voltageNum][asadNum][sectionNum];

    for(int file=0; file<fileNum; file++){
        InFile[file] = new TFile(Form("./hitHisto_%i.root",FileRunName[file]), "read");
        int voltIdx = (FileVoltage[file] == 2000)? 0 : ((FileVoltage[file] == 2100)? 1 : 2);
        int asadIdx = FileAsAdidx[file];

        for(int i=0; i<4; i++){
            hist[voltIdx][asadIdx][i] = (TH1D*)InFile[file] -> Get(Form("hSumADC%i", i));
            // hist[voltIdx][asadIdx][i] -> Rebin(10);

            if(hist[voltIdx][asadIdx][i]){cout << voltIdx << " " << asadIdx << " " << i << " ok! " << endl;}
            // hist[voltIdx][asadIdx][i] -> Scale(1./ hist[voltIdx][asadIdx][i]->GetEntries());
        }
    }

    TCanvas* c1 = new TCanvas("c1", "", 800, 600);

    int cIdx = 0;

    // hist[0][0][1] -> GetYaxis() -> SetRangeUser(0., 1.);
    // auto xaxis = hist[0][0][1] -> GetXaxis();
    // xaxis -> SetRangeUser(0., 6000);

    TH1D* testS0 = new TH1D("testS0", "", 100, 0, 6000);
    TH1D* testS1 = new TH1D("testS1", "", 100, 0, 6000);
    TH1D* testS2 = new TH1D("testS2", "", 100, 0, 6000);

    for(int i=0; i<hist[0][1][1]->GetXaxis()->GetNbins(); i++){
        for(int s0=0; s0<hist[0][1][1]->GetBinContent(i+1); s0++){
            testS0 ->Fill(hist[0][1][1]->GetXaxis()->GetBinCenter(i+1)*0.9);
        }
    }
    for(int i=0; i<hist[0][1][2]->GetXaxis()->GetNbins(); i++){
        for(int s0=0; s0<hist[0][1][2]->GetBinContent(i+1); s0++){
            testS1 ->Fill(hist[0][1][2]->GetXaxis()->GetBinCenter(i+1)*1.9);
        }
    }
    for(int i=0; i<hist[0][1][3]->GetXaxis()->GetNbins(); i++){
        for(int s0=0; s0<hist[0][1][3]->GetBinContent(i+1); s0++){
            testS2 ->Fill(hist[0][1][3]->GetXaxis()->GetBinCenter(i+1)*0.9);
        }
    }


    testS0 -> Scale(1./ testS0->GetEntries());
    testS1 -> Scale(1./ testS1->GetEntries());
    testS2 -> Scale(1./ testS2->GetEntries());

    // hist[0][1][1] -> SetStats(0);
    // hist[0][1][1] -> SetTitle("; ADC; 1/N_{Event} N");
    // hist[0][1][1] -> SetLineColor(kBlack);
    // hist[0][1][1] -> SetLineWidth(2);

    // hist[0][1][2] -> SetLineWidth(2);
    // hist[0][1][2] -> SetLineColor(kRed);

    // hist[0][1][3] -> SetLineWidth(2);
    // hist[0][1][3] -> SetLineColor(kBlue);

    // hist[0][1][1] -> Draw("hist");
    // hist[0][1][2] -> Draw("hist same");
    // hist[0][1][3] -> Draw("hist same");

    testS0 -> SetStats(0);
    testS0 -> SetTitle("; ADC; 1/N_{Event} N");
    testS0 -> SetLineColor(kBlack);
    testS0 -> SetLineWidth(2);

    testS1 -> SetLineWidth(2);
    testS1 -> SetLineColor(kRed);

    testS2 -> SetLineWidth(2);
    testS2 -> SetLineColor(kBlue);

    testS0 -> Draw("hist");
    testS1 -> Draw("hist same");
    testS2 -> Draw("hist same");

    TLegend* leg = new TLegend(0.7, 0.57, 0.89, 0.78);
    leg -> AddEntry(hist[0][1][1], "Section 0");
    leg -> AddEntry(hist[0][1][2], "Section 1");
    leg -> AddEntry(hist[0][1][3], "Section 2");

    TLatex* latex = new TLatex();
    latex -> DrawLatexNDC(0.45, 0.84, "TPC-Drum Fe55 source test");
    latex -> SetTextSize(0.045);
    latex -> DrawLatexNDC(0.45, 0.79, "@ AsAd1, 2000 V");

    leg -> Draw("same");

    // for(int sec=1; sec<sectionNum; sec++){
    //     const int c1Idx = cIdx+1;
    //     c1 -> cd(c1Idx);
    //     for(int asad=0; asad<3; asad++){
    //         for(int volt=0; volt<voltageNum; volt++){
    //             TString drawOpt = (volt == 0 && asad == 0)? "hist" : "same hist";


    //             hist[volt][asad][sec] -> Draw(drawOpt);
    //         }
    //     }
    //     cIdx++;
    // }
}