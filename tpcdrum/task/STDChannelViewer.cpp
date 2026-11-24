#include "STDChannelViewer.h"

ClassImp(STDChannelViewer);

STDChannelViewer::STDChannelViewer()
: fIsOnEventFigure(false), fRunNum("non")
{
    fName = "STDChannelViewer";
}

bool STDChannelViewer::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();
    fSiArray = (STDSiArray*) fDetector -> GetDetectorPlane(1);
    fChannelArray = fRun -> GetBranchA("RawPad");

    hPadPoly = fPadPlane -> GetPadPlanePoly();
    hPadPoly -> GetZaxis() -> SetRangeUser(0., 4000);

    hPadPolyTotal = (TH2Poly*)hPadPoly -> Clone("totalTPC");

    for(int si=0; si<fSiArray->GetSiNum(); si++){
        hSiPoly[si] = fSiArray -> GetSiPoly(si);
        hSiPoly[si] -> GetZaxis()->SetRangeUser(0., 5000.);

        hSiPolyTotal[si] = (TH2Poly*)hSiPoly[si] -> Clone(Form("totalSi_%i", si));
    }

    gSiPos = new TGraph();
    gSiPos -> SetMarkerColor(kRed);
    gSiPos -> SetMarkerStyle(20);
    gSiPos -> SetMarkerSize(1.3);

    double x[4] = {-100., -100., 100., 100.};
    double y[4] = {-34., 166., 166., -34.};
    double z[4] = {0., 230., 230., 0.};

    hBasePadPoly = new TH2Poly();
    hBasePadPoly -> SetStats(0);
    hBasePadPoly -> SetTitle("TPC-Drum pad plane ; x [mm]; y [mm]");
    hBasePadPoly -> AddBin(4, x, y);

    fBaseSiPoly = new TH2Poly();
    fBaseSiPoly -> SetStats(0);
    fBaseSiPoly -> SetTitle("Si Array plane; x [mm]; z [mm]");
    fBaseSiPoly -> GetZaxis()->SetRangeUser(1., 5000.);
    fBaseSiPoly -> AddBin(4, x, z);

    cEvent = new TCanvas("cEvent", "", 600*3., 600.*2.);
    cEvent -> Divide(3,2);
    
    cTatex = new TLatex();
    cTatex ->SetTextSize(0.04);

    for(int i=0; i<3; i++){
        hChannel[i] = new TH2D(Form("hChannel_%i", i),"",250, 0, 512, 1000, -200., 3600.);
        hChannel[i] -> SetStats(0);
    }
    hChannel[0] -> SetTitle(Form("All channel For TPC;TB;ADC"));
    hChannel[1] -> SetTitle(Form("All channel For Si-Ohmic;TB;ADC"));
    hChannel[2] -> SetTitle(Form("All channel For Si-Junction;TB;ADC"));

    hHitNum = new TH1D("hHitNum","", 100, 0, 100);
    hHitNum -> SetTitle("TPC Hit (ADC>60) distribution; Hit; Counts");
    hHitNum -> SetStats(0);

    hSumADC = new TH1D("hHitNum","", 100, 0, 35000);
    hSumADC -> SetTitle("Sum of ADC distribution; Sum ADC; Counts");
    hSumADC -> SetStats(0);

    if(fRunNum == "non"){fRunNum = "Temporary";}

    for(int i=0; i<4; i++){
        for(int j=0; j<68; j++){
            hChanTest[i][j] = new TH1D(Form("hChanTest_AGET%i_CHAN%i", i, j), "", 490, 10, 500);
            hChanTest[i][j] -> SetTitle(Form("All channel pulse at AGET %i CHAN %i;TB;ADC", i, j));
            hChanTest[i][j] -> SetStats(0);
        }
    }

    return true;
}

void STDChannelViewer::Exec(Option_t *option)
{
    if(fIsOnEventFigure){
        for(int i=0; i<3; i++){
            hChannel[i] -> Reset("ICESM");
        }
        hPadPoly -> ClearBinContents();
        for(int i=0; i<8; i++){hSiPoly[i]->ClearBinContents();}
    }
    gSiPos -> Set(0);

    vector<tuple<int, int, int>> tpcHitADC[3]; // [asad][aget, chan, maxADC]
    vector<tuple<int, int, int>> tpcHitTB[3]; // [asad][aget, chan, maxTB]

    vector<pair<int, int>> ohmicHit[2]; // [adc , tb][chan, values]
    vector<tuple<int, int, int>> juncHit[2]; // [adc, tb][chan, values]

    // Step1: semi-noise subtractions
    const int channelNum = fChannelArray -> GetEntries();
    for(int chan=0; chan<channelNum; chan++){
        fChannel = (GETChannel*)fChannelArray -> At(chan);
        auto rawADCArr = fChannel -> GetWaveformY();

        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}

        bool isSiArray = (3 == asadID)? true : false;
        if(isSiArray && agetID == 3){continue;}
        bool isSiOhmic = (isSiArray && agetID == 0)? true : false;
        bool isSiJunction = (isSiArray && agetID == 1 || agetID == 2)? true : false;
        
        // for si-TPC integrating
        // int startTBHit = (isSiArray)? 20 : 250;
        // int endTBHit = (isSiArray)? 70 : 400;   
        // int startTBPede = (isSiArray)? 150 : 50;
        // int endTBPede = (isSiArray)? 250 : 100;

        int startTBHit = (isSiArray)? 20 : 20;
        int endTBHit = (isSiArray)? 70 : 350;   
        int startTBPede = (isSiArray)? 150 : 400;
        int endTBPede = (isSiArray)? 250 : 500;

        int maxADC = 0.;
        int maxTB = 0;
        for(int tb=startTBHit; tb<endTBHit; tb++){
            double adc = rawADCArr[tb];
            if(isSiArray && isSiJunction){adc = -adc;}
            if(maxADC < adc){
                maxADC = adc;
                maxTB = tb;
            }
        }
        double TBNum = endTBPede-startTBPede;
        double avgPedestal = 0.;
        for(int tb=startTBPede; tb<endTBPede; tb++){
            double adc = rawADCArr[tb];
            if(isSiArray && isSiJunction){adc = -adc;}
            avgPedestal += adc;
        }
        avgPedestal /= TBNum;
        // maxADC -= avgPedestal;

        if(fIsOnEventFigure){
            for(int tb=10; tb<500; tb++){

                // double adc = rawADCArr[tb]-avgPedestal;
                double adc = rawADCArr[tb];

                if(!isSiArray){
                    hChannel[0] -> Fill(tb+1, adc);
                }
                if(isSiArray && isSiOhmic){
                    hChannel[1] -> Fill(tb+1, adc);
                }
                if(isSiArray && isSiJunction){
                    adc = -rawADCArr[tb]-avgPedestal;
                    hChannel[2] -> Fill(tb+1, adc);
                }
            }
        }

        if(maxADC < 60.){continue;} // at least minimum cut for subtracted Max ADC over 60

        if(!isSiArray){
            if(maxTB > 480. || maxTB < 20){continue;}
            tpcHitADC[asadID].push_back(make_tuple(agetID, chanID, maxADC));
            tpcHitTB[asadID].push_back(make_tuple(agetID, chanID, maxTB));

            int padID = fPadPlane -> GetPadID(asadID, agetID, chanID);
            if(fIsOnEventFigure){
                double x = fPadPlane -> GetX(padID);
                double y = fPadPlane -> GetY(padID);
                hPadPoly -> Fill(x, y, maxADC);
            }

            int binContent = hPadPolyTotal -> GetBinContent(padID+1);
            hPadPolyTotal -> SetBinContent(padID+1, binContent+1);
        }   

        if(maxTB < startTBHit+1){continue;}

        if(isSiArray && isSiOhmic){
            ohmicHit[0].push_back(make_pair(chanID, maxADC));
            ohmicHit[1].push_back(make_pair(chanID, maxTB));
        }
        if(isSiArray && isSiJunction){
            juncHit[0].push_back(make_tuple(agetID, chanID, maxADC));
            juncHit[1].push_back(make_tuple(agetID, chanID, maxTB));
        }
    }
    int ohmicHitNum = ohmicHit[0].size();
    int juncHitNum = juncHit[0].size(); 
    // if(ohmicHitNum == 0 || juncHitNum < 2){return;}

    // Si pair matching
    vector<pair<int, int>> juncPairIdx;
    for(int j1=0; j1<juncHitNum-1; j1++){
        int agetID1 = get<0>(juncHit[0][j1]);
        int chanID1 = get<1>(juncHit[0][j1]);
        int siID1 = fSiArray -> GetSiDetID(agetID1, chanID1);
        int stripID1 = fSiArray -> GetStripID(agetID1, chanID1);

        for(int j2=j1+1; j2<juncHitNum; j2++){
            int agetID2 = get<0>(juncHit[0][j2]);
            int chanID2 = get<1>(juncHit[0][j2]);
            int siID2 = fSiArray -> GetSiDetID(agetID2, chanID2);
            int stripID2 = fSiArray -> GetStripID(agetID2, chanID2);

            if(siID1 != siID2){continue;}
            if(stripID1 != stripID2){continue;}
            juncPairIdx.push_back(make_pair(j1, j2));
        }   
    }
    int juncPairNum = juncPairIdx.size();
    // if(juncPairNum == 0){return;}

    vector<int> totalSiADC;

    // si ohmic-junction pair matching & reco position
    for(int o=0; o<ohmicHitNum; o++){
        int chanIDOhmic = ohmicHit[0][o].first;
        int ohmicADC = ohmicHit[0][o].second;
        int ohmicTB = ohmicHit[1][o].second;
        int siIDOhmic = fSiArray -> GetSiDetID(0, chanIDOhmic);
        int ohmicID = fSiArray -> GetOhmicID(0, chanIDOhmic);

        for(int j=0; j<juncPairNum; j++){
            int idxJunc1 = juncPairIdx[j].first;
            int idxJunc2 = juncPairIdx[j].second;
            int agetIDJunc1 = get<0>(juncHit[0][idxJunc1]);
            int chanIDJunc1 = get<1>(juncHit[0][idxJunc1]);
            int siIDJunc1 = fSiArray -> GetSiDetID(agetIDJunc1, chanIDJunc1);
            if(siIDOhmic != siIDJunc1){continue;}

            int junc1ADC = get<2>(juncHit[0][idxJunc1]);
            int junc2ADC = get<2>(juncHit[0][idxJunc2]);
            int junc1TB = get<2>(juncHit[1][idxJunc1]);
            int junc2TB = get<2>(juncHit[1][idxJunc2]);

            if(fabs(ohmicTB - junc1TB) > 30.){continue;}
            int totalADC = junc1ADC+junc2ADC+ohmicADC;
            int junc1ID = fSiArray -> GetJuncID(agetIDJunc1, chanIDJunc1);
            int unitPadID = fSiArray -> GetUnitPadID(ohmicID, junc1ID);
            
            if(fIsOnEventFigure){
                hSiPoly[siIDOhmic] -> SetBinContent(unitPadID+1, totalADC);
            }

            totalSiADC.push_back(totalADC);

            if(fIsOnEventFigure){
                hSiPolyTotal[siIDOhmic] -> SetBinContent(unitPadID+1, hSiPolyTotal[siIDOhmic]->GetBinContent(unitPadID+1)+1);
            }
        }
    }

    // ================== Si-Junction Position ================
    for(int j=0; j<juncPairNum; j++){
        int idxJunc1 = juncPairIdx[j].first;
        int idxJunc2 = juncPairIdx[j].second;
        int agetIDJunc1 = get<0>(juncHit[0][idxJunc1]);
        int chanIDJunc1 = get<1>(juncHit[0][idxJunc1]);
        int agetIDJunc2 = get<0>(juncHit[0][idxJunc2]);
        int chanIDJunc2 = get<1>(juncHit[0][idxJunc2]);
        int siIDJunc1 = fSiArray -> GetSiDetID(agetIDJunc1, chanIDJunc1);

        double junc1ADC = get<2>(juncHit[0][idxJunc1]);
        double junc2ADC = get<2>(juncHit[0][idxJunc2]);
        double junc1TB = get<2>(juncHit[1][idxJunc1]);
        double junc2TB = get<2>(juncHit[1][idxJunc2]);

        int stripID = fSiArray -> GetStripID(agetIDJunc1, chanIDJunc1);
        int junc1ID = fSiArray -> GetJuncID(agetIDJunc1, chanIDJunc1);
        int junc2ID = fSiArray -> GetJuncID(agetIDJunc2, chanIDJunc2);
        bool isStartPosChan = (junc1ID%2 == 0)?  true : false;

        double ratioE = junc2ADC/(junc1ADC+junc2ADC);
        if(!isStartPosChan){ratioE = junc1ADC/(junc1ADC+junc2ADC);}
        const double siLength = fSiArray -> GetSiHeight();

        double x = ratioE * siLength;
        double y = 0.;
        double stripNum = fSiArray -> GetSiStripNum();
        double siWidth = fSiArray -> GetSiWidth();
        double z = stripID * (siWidth/stripNum)*3./2.; 

        // fSiArray -> ConvertSiLocalPos2Pad(siIDJunc1, x, y, z); 

        // if(fabs(junc1TB - junc2TB) < 30.){
        //     gSiPos -> SetPoint(0, x, z);

        cout << " ============ before " << x << " " << z << endl;
        cout << siIDJunc1 << " " << fSiArray -> GetSiDetID(agetIDJunc2, chanIDJunc2) <<  " " << stripID << " "<< junc1ID << " "<< junc2ID << endl;
        
        fSiArray -> ConvertSiLocalPos2Pad(siIDJunc1, x, y, z); 
        cout << x << " " << z << endl;
        // }

    }

    // ============ TPC tracking only for AsAd 2 ===============
    int tpcHitNum = tpcHitADC[2].size();
    // if(tpcHitNum > 5){
    //     double layerWeight[3][2][2]; // [layer][posX, posZ][pos*w, sumW]
    //     memset(layerWeight, 0., sizeof(layerWeight));
    //     for(int i=0; i<tpcHitNum; i++){
    //         int aget =  get<0>(tpcHitADC[2][i]);
    //         int chan =  get<1>(tpcHitADC[2][i]);
    //         double adc =  get<2>(tpcHitADC[2][i]);
    //         double tb =  get<2>(tpcHitTB[2][i]);
    //         int layer = fPadPlane -> GetLayerID(2, aget, chan);
    //         int row = fPadPlane -> GetRowID(2, aget, chan);
    //         double x = fPadPlane -> GetX(layer, row);
    //         layerWeight[11-layer][0][0] += x*adc;
    //         layerWeight[11-layer][0][1] += adc;
    //         layerWeight[11-layer][1][0] += tb*adc;
    //         layerWeight[11-layer][1][1] += adc;
    //     }

        
    // }


    // vector<tuple<int, int, int>> tpcHitADC[3]; // [asad][aget, chan, maxADC]
    // vector<tuple<int, int, int>> tpcHitTB[3]; // [asad][aget, chan, maxTB]

    // vector<pair<int, int>> ohmicHit[2]; // [adc , tb][chan, values]
    // vector<tuple<int, int, int>> juncHit[2]; // [adc, tb][chan, values]

    if(fIsOnEventFigure){
        hHitNum -> Fill(tpcHitNum);    
        if(tpcHitNum > 13){
            double totalADC = 0.;
            for(int i=0; i<tpcHitNum; i++){
                totalADC += get<2>(tpcHitADC[2][i]);
            }
            for(int i=0; i<totalSiADC.size(); i++){
                // totalADC += totalSiADC[i];
                hSumADC -> Fill(totalADC+ totalSiADC[i]);
            }

        }

        cEvent -> cd(1);
        gPad -> SetLogz();
        fBaseSiPoly -> Draw("");
        for(int i=0; i<8; i++){
            hSiPoly[i] -> Draw("colz, same");
        }
        gSiPos -> Draw("same p");

        cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));
        cTatex -> DrawLatexNDC(0.13, 0.805, Form("Event %i", fRun->GetCurrentEventID()));

        cEvent -> cd(4);
        // gPad -> SetLogz();
        hBasePadPoly -> Draw("");
        hPadPoly -> Draw("colz, same");
        cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));
        cTatex -> DrawLatexNDC(0.13, 0.805, Form("Event %i", fRun->GetCurrentEventID()));

        cEvent -> cd(2);
        hChannel[0] -> Draw("colz");
        cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));
        cTatex -> DrawLatexNDC(0.13, 0.805, Form("Event %i", fRun->GetCurrentEventID()));

        cEvent -> cd(3);
        hChannel[1] -> Draw("colz");
        hChannel[1]->GetYaxis()->SetRangeUser(-200., 1000.);
        cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));
        cTatex -> DrawLatexNDC(0.13, 0.805, Form("Event %i", fRun->GetCurrentEventID()));

        cEvent -> cd(6);
        hChannel[2] -> Draw("colz");
        hChannel[2]->GetYaxis()->SetRangeUser(-200., 1000.);
        cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));
        cTatex -> DrawLatexNDC(0.13, 0.805, Form("Event %i", fRun->GetCurrentEventID()));
        
        cEvent -> Draw();
        cEvent -> SaveAs(Form("./figure/Run%s_event%i_channel.png", fRunNum.Data(), fRun->GetCurrentEventID()));
    }

}

bool STDChannelViewer::EndOfRun()
{
    TCanvas* c1 = new TCanvas("","",1200,1200);
    c1 -> Divide(2,2);

    c1 -> cd(1);
    hHitNum -> GetYaxis()->SetRangeUser(0., hHitNum->GetMaximum()*1.5);
    hHitNum -> Draw();
    cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));

    c1 -> cd(3);
    hSumADC -> GetYaxis()->SetRangeUser(0., hSumADC->GetMaximum()*1.5);
    hSumADC -> Draw();
    cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));

    c1 -> cd(2);
    gPad -> SetLogz();
    fBaseSiPoly -> Draw("");
    for(int i=0; i<8; i++){
        hSiPolyTotal[i] -> Draw("colz, same");
    }
    cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));

    c1 -> cd(4);
    gPad -> SetLogz();
    hBasePadPoly -> Draw("");
    hPadPolyTotal -> Draw("colz, same");
    cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));

    c1 -> Draw();
    c1 -> SaveAs(Form("./figure/RunSummary_run%s.pdf", fRunNum.Data()));

    return true;
}