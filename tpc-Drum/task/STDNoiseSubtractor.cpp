#include "STDNoiseSubtractor.h"

ClassImp(STDNoiseSubtractor);

STDNoiseSubtractor::STDNoiseSubtractor()
{
    fName = "STDNoiseSubtractor";
}

bool STDNoiseSubtractor::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();
    fPadArray = fRun -> GetBranchA("RawPad");

    fADCTmp = new TH1D("ADCTmp", "", 512, 0, 512);
    for(int asad=0; asad<4; asad++){
        fADCTemplate[asad] = new TH2D(Form("asad_%i", asad), "", 512, 0, 512, 2000, 0., 2000.);
    }

    return true;
}

void STDNoiseSubtractor::Exec(Option_t *option)
{
    const int asadNum = fAsAdNum;

    double tmpADC[256][512];
    memset(tmpADC, 0, sizeof(tmpADC));

    double tmpPadNum[asadNum]; // [asad][padNum]
    memset(tmpPadNum, 0., sizeof(tmpPadNum));

    const int tbIdxNum = fTBIdxNum;
    double tmpTempADC[asadNum][tbIdxNum][2]; // [asad][tbIdx][mean, StdDev]
    memset(tmpTempADC, 0., sizeof(tmpTempADC));

    const int padNum = fPadArray -> GetEntries();
    for(int pad=0; pad<padNum; pad++){
        fPad = (LKPad*)fPadArray -> At(pad);
        auto rawADCArr = fPad -> GetArrayRawSig();

        int asadID = fPad -> GetAsad();
        int padID = fPad -> GetPadID();
        if(fDetector -> IsDeadChan(0, padID)){continue;} // dead Channel 

        // Find a Mean of ADC by Pad
        double tmpADCMean = 0.;
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            tmpADCMean += double(rawADCArr[tb]);
        }
        tmpADCMean /= double(fTBEndIdx - fTBStartIdx);

        // Subtract the ADC mean, added ADC offset into tmpADC for analize
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            tmpADC[padID][tb] = double(rawADCArr[tb]) + fTmpADCOffset - tmpADCMean;
        }

        for(int tbIdx=0; tbIdx<fTBIdxNum; tbIdx++){
            int tb = (tbIdx+1)*fTBInterval;
            tmpTempADC[asadID][tbIdx][0] += tmpADC[padID][tbIdx];

        }
        tmpPadNum[asadID] += 1.;
    }

    for(int pad=0; pad<padNum; pad++){
        fPad = (LKPad*)fPadArray -> At(pad);
        int asadID = fPad -> GetAsad();
        int padID = fPad -> GetPadID();
        if(fDetector -> IsDeadChan(0, padID)){continue;}

        for(int tbIdx=0; tbIdx<fTBIdxNum; tbIdx++){
            double mean = tmpTempADC[asadID][tbIdx][0]/tmpPadNum[asadID];
            double adc = tmpADC[padID][tbIdx];

            tmpTempADC[asadID][tbIdx][1] += (adc - mean)*(adc - mean);
        }
    }

    for(int asad=0; asad<fAsAdNum; asad++){
        fADCTemplate[asad] -> Reset("ICESM");
    }

    for(int pad=0; pad<padNum; pad++){
        fPad = (LKPad*)fPadArray -> At(pad);
        int asadID = fPad -> GetAsad();
        int padID = fPad -> GetPadID();
        if(fDetector -> IsDeadChan(0, padID)){continue;}

        int rejectIdx = 0;

        for(int tbIdx=0; tbIdx<fTBIdxNum; tbIdx++){
            double mean = tmpTempADC[asadID][tbIdx][0]/tmpPadNum[asadID];
            double stdDev = sqrt(tmpTempADC[asadID][tbIdx][1]/(tmpPadNum[asadID]-1.));
            if(fabs(tmpADC[padID][tbIdx] > mean + 1.25*stdDev)){rejectIdx++;}
        }

        if(rejectIdx > 2){continue;} // cut the pulse-like-pad

        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            fADCTemplate[asadID] -> Fill(tb, tmpADC[padID][tb]);
        }
    }

    for(int asad=0; asad<asadNum; asad++){
        fNoiseTemplate[asad] = (TProfile*)fADCTemplate[asad] -> ProfileX();
    }

    for(int pad=0; pad<padNum; pad++){
        fPad = (LKPad*)fPadArray -> At(pad);
        int asadID = fPad -> GetAsad();
        int padID = fPad -> GetPadID();
        if(fDetector -> IsDeadChan(0, padID)){continue;}
        auto rawADCArr = fPad -> GetArrayRawSig();

        fADCTmp -> Reset("ICESM");
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            fADCTmp -> SetBinContent(tb+1, double(rawADCArr[tb])+fTmpADCOffset);
        }

        double adcIntegral = fADCTmp->Integral(fTBStartIdx, 80) + fADCTmp->Integral(470, fTBEndIdx);
        double noiseIntegral = fNoiseTemplate[asadID]->Integral(fTBStartIdx, 80) + fNoiseTemplate[asadID]->Integral(470, fTBEndIdx);
        fNoiseTemplate[asadID] -> Scale(adcIntegral/noiseIntegral);

        int tmpADC2[512];
        memset(tmpADC2, 0, sizeof(tmpADC2));
        for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
            double adc = fADCTmp -> GetBinContent(tb+1) - fTmpADCOffset;
            double noise =  fNoiseTemplate[asadID] -> GetBinContent(tb+1) - fTmpADCOffset;
            tmpADC2[tb] = int(adc - noise);
        }
        fPad -> SetBufferRawSig(tmpADC2);
    }
}

bool STDNoiseSubtractor::EndOfRun()
{
    return true;
}
