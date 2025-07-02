#include "STDNoiseSubtractor.h"

ClassImp(STDNoiseSubtractor);

STDNoiseSubtractor::STDNoiseSubtractor()
{
    fName = "STDNoiseSubtractor";
}

bool STDNoiseSubtractor::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fDetectorPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();
    fChannelArray = fRun -> GetBranchA("RawPad");

    fADCTmp = new TH1D("ADCTmp", "", 512, 0, 512);
    for(int asad=0; asad<4; asad++){
        fADCTemplate[asad] = new TH2D(Form("asad_%i", asad), "", 512, 0, 512, 2000, 0., 2000.);
    }


    // test
    cADC = new TCanvas("cADC", "cADC", 600, 600);
    cADCAsad = new TCanvas("cADCasad","cADCasad", 1200,1200);
    cADCAsad -> Divide(2,2); 
    for(int asad=0; asad<4; asad++){
        for(int aget=0; aget<4; aget++){
            for(int chan=0; chan<68; chan++){
                hRawADC[asad][aget][chan] = new TH1D(Form("rawADC_%i_%i_%i", asad, aget, chan), "", 512, 0, 512);
                hRawADC[asad][aget][chan] -> SetLineColor(kBlue);
                hRawADC[asad][aget][chan] -> GetYaxis()->SetRangeUser(-50., 1000.);
                hSubADC[asad][aget][chan] = new TH1D(Form("subADC_%i_%i_%i", asad, aget, chan), "", 512, 0, 512);
                hSubADC[asad][aget][chan] -> SetLineColor(kRed);
                hSubADC[asad][aget][chan] -> GetYaxis()->SetRangeUser(-50., 3000.);
            }
        }
    }


    return true;
}

void STDNoiseSubtractor::Exec(Option_t *option)
{
    // if(fRun->GetCurrentEventID() != 16 &&fRun->GetCurrentEventID() != 18){return;}
    const int asadNum = 4;

    // test !! Todo fixed a tmpADC and noise template method 25/02/28

    double tmpADC[ASADNUM][AGETNUM][CHANNUM][TIMEBUCKET];
    memset(tmpADC, 0, sizeof(tmpADC));

    double tmpPadNum[asadNum]; // [asad][padNum]
    memset(tmpPadNum, 0., sizeof(tmpPadNum));

    const int tbIdxNum = fTBIdxNum;
    double tmpTempADC[asadNum][tbIdxNum][2]; // [asad][tbIdx][mean, StdDev]
    memset(tmpTempADC, 0., sizeof(tmpTempADC));

    const int padNum = fChannelArray -> GetEntries();
    for(int pad=0; pad<padNum; pad++){
        fChannel = (GETChannel*)fChannelArray -> At(pad);
        auto rawADCArr = fChannel -> GetWaveformY();

        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}

        for(int tb=0; tb<512; tb++){
            // test
            hRawADC[asadID][agetID][chanID] -> SetBinContent(tb+1, rawADCArr[tb]);
        }

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
        tmpPadNum[asadID] += 1.;
    }

    for(int pad=0; pad<padNum; pad++){
        fChannel = (GETChannel*)fChannelArray -> At(pad);
        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}

        for(int tbIdx=0; tbIdx<fTBIdxNum; tbIdx++){
            int tb = (tbIdx+1)*fTBInterval;
            double mean = tmpTempADC[asadID][tbIdx][0]/tmpPadNum[asadID];
            double adc = tmpADC[asadID][agetID][chanID][tb];

            tmpTempADC[asadID][tbIdx][1] += (adc - mean)*(adc - mean);
        }
    }

    for(int asad=0; asad<asadNum; asad++){
        fADCTemplate[asad] -> Reset("ICESM");
    }

    for(int pad=0; pad<padNum; pad++){
        fChannel = (GETChannel*)fChannelArray -> At(pad);
        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}

        int rejectIdx = 0;
        for(int tbIdx=0; tbIdx<fTBIdxNum; tbIdx++){
            int tb = (tbIdx+1)*fTBInterval;
            double mean = tmpTempADC[asadID][tbIdx][0]/tmpPadNum[asadID];
            double stdDev = sqrt(tmpTempADC[asadID][tbIdx][1]/(tmpPadNum[asadID]-1.));
            if(fabs(tmpADC[asadID][agetID][chanID][tb] > mean + 1.5*stdDev)){rejectIdx++;}
        }

        if(rejectIdx > 2){continue;} // cut the pulse-like-pad

        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            fADCTemplate[asadID] -> Fill(tb, tmpADC[asadID][agetID][chanID][tb]);
        }
    }

    for(int asad=0; asad<asadNum; asad++){
        fNoiseTemplate[asad] = (TProfile*)fADCTemplate[asad] -> ProfileX();
    }

    for(int pad=0; pad<padNum; pad++){
        fChannel = (GETChannel*)fChannelArray -> At(pad);
        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}

        auto rawADCArr = fChannel -> GetWaveformY();

        fADCTmp -> Reset("ICESM");
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            fADCTmp -> SetBinContent(tb+1, double(rawADCArr[tb])+fTmpADCOffset);
        }

        double adcIntegral = fADCTmp->Integral(fTBStartIdx, 40) + fADCTmp->Integral(470, fTBEndIdx);
        double noiseIntegral = fNoiseTemplate[asadID]->Integral(fTBStartIdx, 40) + fNoiseTemplate[asadID]->Integral(470, fTBEndIdx);
        fNoiseTemplate[asadID] -> Scale(adcIntegral/noiseIntegral);

        int tmpADC2[512];
        memset(tmpADC2, 0, sizeof(tmpADC2));
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            double adc = fADCTmp -> GetBinContent(tb+1) - fTmpADCOffset;
            double noise =  fNoiseTemplate[asadID] -> GetBinContent(tb+1) - fTmpADCOffset;
            tmpADC2[tb] = int(adc - noise);

            hSubADC[asadID][agetID][chanID] -> SetBinContent(tb, tmpADC2[tb]);
        }
        fChannel -> SetWaveformY(tmpADC2);
    }

    // for(int asad=0; asad<1; asad++){
    //     for(int aget=0; aget<4; aget++){
    //         for(int chan=0; chan<68; chan++){
    //             cADC -> cd();
    //             hRawADC[asad][aget][chan] -> Draw("hist");
    //             hSubADC[asad][aget][chan] -> Draw("hist, same");

    //             cADC -> Draw();
    //             cADC -> SaveAs(Form("./pulse/Pulse_Event%i_asad%i_aget%i_chan%i.pdf", int(fRun->GetCurrentEventID()), asad, aget, chan));
    //         }
    //     }
    // }

    // for(int asad=0; asad<1; asad++){
    //     const int cIdx = asad+1;
    //     cADCAsad -> cd(cIdx);
    //     // hRawADC[asad][0][0] -> Draw("hist");
    //     hSubADC[asad][0][0] -> Draw("hist");
    //     for(int aget=0; aget<3; aget++){
    //         for(int chan=0; chan<68; chan++){
    //             // hRawADC[asad][aget][chan] -> Draw("hist, same");
    //             hSubADC[asad][aget][chan] -> Draw("hist, same");
    //         }
    //     }
    // }
    // cADCAsad -> Draw();
    // cADCAsad -> SaveAs(Form("./pulse/Pulse_Event%i.pdf", fRun->GetCurrentEventID()));


}

bool STDNoiseSubtractor::EndOfRun()
{
    return true;
}
