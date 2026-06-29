#include "STDNoiseSubtractor.h"

ClassImp(STDNoiseSubtractor);

STDNoiseSubtractor::STDNoiseSubtractor()
: fMakeNoiseShapeMode(false), fOnDrawRawADC(false)
{
    fName = "STDNoiseSubtractor";
}

bool STDNoiseSubtractor::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane(0);
    fSiArray = (STDSiArray*) fDetector -> GetDetectorPlane(1);

    fChannelArray = fRun -> GetBranchA("RawPad");

    fSiArrayAsAdID = 3; 
    if(fPar -> CheckPar("TPCDrum/SiArrayAsAdID")){
        fSiArrayAsAdID = fPar -> GetParInt("TPCDrum/SiArrayAsAdID");
    }

    fADCTmp = new TH1D("ADCTmp", "", 512, 0, 512);
    for(int asad=0; asad<4; asad++){
        fADCTemplate[asad] = new TH2D(Form("asad_%i", asad), "", 512, 0, 512, 2000, 0., 2000.);
    }

    if(fMakeNoiseShapeMode){
        TString currentPath = gSystem -> pwd();
        int projectPathIndex = currentPath.Index("tpc-Drum/");
        currentPath.Remove(projectPathIndex);
        TString noiseDataPath = currentPath + "tpc-Drum/common/";

        TString noiseDataName = Form("%sNoise_template.root", noiseDataPath.Data());
        fNoiseFile = new TFile(noiseDataName, "recreate");
        fNoiseTree = new TTree("event", "event");
        fNoiseTree -> Branch("Noise", fNoiseData, Form("Noise[%i]/D", TIMEBUCKET));
    }

    if(fOnDrawRawADC){
        cout << "STDNoiseSubtractor::Init() -- " << "Drawing mode ON "<< endl;

        hTB_subt = new TH2D("TB_subt","", 512, 0, 512, 3600, -100, 3500);
        hTB_subt -> SetTitle("Timebucket summary; TB ; ADC");

        cTestCanvas = new TCanvas("cTestCanvas","",600., 600.);
        hTestTB = new TH1D("hTestTB", "", 512, 0., 512.);
        hTestTB -> SetStats(0);
        hTestRawTB = new TH1D("hTestTB", "", 512, 0., 512.);
        hTestRawTB -> SetLineColor(kRed);
        hTestRawTB -> SetStats(0);
    }

    return true;
}

void STDNoiseSubtractor::Exec(Option_t *option)
{
    // test !! Todo fixed a tmpADC and noise template method 25/02/28

    double tmpADC[ASADNUM][AGETNUM][CHANNUM][TIMEBUCKET];
    memset(tmpADC, 0, sizeof(tmpADC));

    double tmpChanNum[ASADNUM];
    memset(tmpChanNum, 0., sizeof(tmpChanNum));

    const int tbIdxNum = fTBIdxNum;
    double tmpTempADC[ASADNUM][tbIdxNum][2]; // [asad][tbIdx][mean, StdDev]
    memset(tmpTempADC, 0., sizeof(tmpTempADC));

    const int channelNum = fChannelArray -> GetEntries();
    for(int chan=0; chan<channelNum; chan++){
        fChannel = (GETChannel*)fChannelArray -> At(chan);
        auto rawADCArr = fChannel -> GetWaveformY();

        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}

        // if(fDetector -> IsDeadChan(0, padID)){continue;} // dead Channel 

        // Find a Mean of ADC by Pad
        double tmpADCMean = 0.;
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            tmpADCMean += double(rawADCArr[tb]);
        }
        tmpADCMean /= double(fTBEndIdx - fTBStartIdx);

        // Subtract the ADC mean, added ADC offset into tmpADC for analize
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            tmpADC[asadID][agetID][chanID][tb] = double(rawADCArr[tb]) + fTmpADCOffset - tmpADCMean;
        }

        for(int tbIdx=0; tbIdx<fTBIdxNum; tbIdx++){
            int tb = (tbIdx+1)*fTBInterval;
            tmpTempADC[asadID][tbIdx][0] += tmpADC[asadID][agetID][chanID][tb];
        }
        tmpChanNum[asadID] += 1.;
    }

    for(int chan=0; chan<channelNum; chan++){
        fChannel = (GETChannel*)fChannelArray -> At(chan);
        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}

        for(int tbIdx=0; tbIdx<fTBIdxNum; tbIdx++){
            int tb = (tbIdx+1)*fTBInterval;
            double mean = tmpTempADC[asadID][tbIdx][0]/tmpChanNum[asadID];
            double adc = tmpADC[asadID][agetID][chanID][tb];

            tmpTempADC[asadID][tbIdx][1] += (adc - mean)*(adc - mean);
        }
    }

    for(int asad=0; asad<ASADNUM; asad++){
        fADCTemplate[asad] -> Reset("ICESM");
    }

    for(int chan=0; chan<channelNum; chan++){
        fChannel = (GETChannel*)fChannelArray -> At(chan);
        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}

        int rejectIdx = 0;
        for(int tbIdx=0; tbIdx<fTBIdxNum; tbIdx++){
            int tb = (tbIdx+1)*fTBInterval;
            double mean = tmpTempADC[asadID][tbIdx][0]/tmpChanNum[asadID];
            double stdDev = sqrt(tmpTempADC[asadID][tbIdx][1]/(tmpChanNum[asadID]-1.));
            if(fabs(tmpADC[asadID][agetID][chanID][tb] > mean + 1.5*stdDev)){rejectIdx++;}
        }

        if(rejectIdx > 2){continue;} // cut the pulse-like-pad

        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            fADCTemplate[asadID] -> Fill(tb, tmpADC[asadID][agetID][chanID][tb]);
        }
    }

    for(int asad=0; asad<ASADNUM; asad++){
        fNoiseTemplate[asad] = (TProfile*)fADCTemplate[asad] -> ProfileX();
    }

    for(int chan=0; chan<channelNum; chan++){
        fChannel = (GETChannel*)fChannelArray -> At(chan);
        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}

        auto rawADCArr = fChannel -> GetWaveformY();

        fADCTmp -> Reset("ICESM");
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            fADCTmp -> SetBinContent(tb+1, double(rawADCArr[tb])+fTmpADCOffset);
        }

        double noiseIntegral = fNoiseTemplate[asadID]->Integral(fTBStartIdx, 40) + fNoiseTemplate[asadID]->Integral(470, fTBEndIdx);
        fNoiseTemplate[asadID] -> Scale(1./noiseIntegral);
        double NoiseOffset = fNoiseTemplate[asadID]->Integral(fTBStartIdx+1, fTBEndIdx-1) / double(fTBEndIdx-1 - fTBStartIdx+1);

        double adcIntegral = fADCTmp->Integral(fTBStartIdx, 40) + fADCTmp->Integral(470, fTBEndIdx);
        fNoiseTemplate[asadID] -> Scale(adcIntegral);

        bool isSiArray = (fSiArrayAsAdID == asadID)? true : false;
        bool isSiJunction = fSiArray -> IsJunction(agetID, chanID);

        int tmpADC2[512];
        memset(tmpADC2, 0, sizeof(tmpADC2));
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            double adc = fADCTmp -> GetBinContent(tb+1) - fTmpADCOffset;
            double noise =  fNoiseTemplate[asadID] -> GetBinContent(tb+1) - fTmpADCOffset - NoiseOffset;
            tmpADC2[tb] = int(adc - noise);

            if(isSiArray && isSiJunction){tmpADC2[tb] = -tmpADC2[tb];}
        }

        if(fOnDrawRawADC){
            int maxADC = 0;
            int maxTime = 0.;

            hTestTB -> Reset("ICESM");
            hTestRawTB -> Reset("ICESM");
            for(int tb=0; tb<512; tb++){
                hTB_subt -> Fill(tb, tmpADC2[tb]);
                hTestTB -> Fill(tb, tmpADC2[tb]);
                hTestRawTB -> Fill(tb, tmpADC[asadID][agetID][chanID][tb]);
                if(maxADC < tmpADC2[tb]){
                    maxADC = tmpADC2[tb];
                    maxTime = tb;
                }
            }

            if(maxADC > 10){
                cTestCanvas -> cd();
                hTestTB -> GetYaxis()->SetRangeUser(-200, 4000.);
                hTestTB -> Draw("hist");
                hTestRawTB -> Draw("same, hist");
                fNoiseTemplate[0] -> SetLineColor(kBlack);
                fNoiseTemplate[0] -> Draw("same, hist");
                cTestCanvas -> Update();
                cTestCanvas -> SaveAs(Form("_ttevent%i_aget%i_chan%i.png", fRun->GetCurrentEventID(), agetID, chanID));
            }
        }
        fChannel -> SetWaveformY(tmpADC2);
    }

    if(fMakeNoiseShapeMode){
        SaveNoiseTemplate();
    }

    // Drawing 
    if(fOnDrawRawADC){
        // hTB_subt -> GetYaxis()->SetRangeUser(-500, 1000.);
        // hTB_subt -> Draw("colz, hist");
    }
}

bool STDNoiseSubtractor::EndOfRun()
{
    if(fMakeNoiseShapeMode){
        fNoiseFile -> cd();
        fNoiseTree -> Write();
        fNoiseFile -> Close();
    }

    return true;
}

void STDNoiseSubtractor::SaveNoiseTemplate()
{
    for(int asad=0; asad<ASADNUM; asad++){
        double noiseIntegral = fNoiseTemplate[asad]->Integral(fTBStartIdx, 40) + fNoiseTemplate[asad]->Integral(470, fTBEndIdx);
        if(noiseIntegral < 0.01){continue;}
        fNoiseTemplate[asad] -> Scale(1./noiseIntegral);
        memset(fNoiseData, 0., sizeof(fNoiseData));

        double NoiseOffset = fNoiseTemplate[asad]->Integral(fTBStartIdx+1, fTBEndIdx-1) / double(fTBEndIdx-1 - fTBStartIdx+1);

        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            fNoiseData[tb] = fNoiseTemplate[asad]->GetBinContent(tb+1) - NoiseOffset;
        }
        fNoiseTree -> Fill();
    }
}