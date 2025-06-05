#include "STDPulseAnalyser.h"

ClassImp(STDPulseAnalyser);

STDPulseAnalyser::STDPulseAnalyser()
: fMethodOpt(0)
{
    fName = "STDPulseAnalyser";
}

bool STDPulseAnalyser::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fDetectorPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();
    fChannelArray = fRun -> GetBranchA("RawPad");
    fHitArray = fRun -> RegisterBranchA("Hit", "LKHit");

    return true;
}

void STDPulseAnalyser::Exec(Option_t *option)
{
    fHitArray -> Clear("C");

    int hitNum = 0;
    int padNum = fChannelArray -> GetEntries();
    for(int pad=0; pad<padNum; pad++){
        fChannel = (GETChannel*)fChannelArray -> At(pad);

        int asadId = fChannel -> GetAsad();
        int agetId = fChannel -> GetAget();
        int chanId = fChannel -> GetChan();
        int padID = fChannel -> GetPadID();

        int layer = fDetectorPlane -> GetLayerID(asadId, agetId, chanId);
        int row = fDetectorPlane -> GetRowID(asadId, agetId, chanId);
        double x = fDetectorPlane -> GetX(layer, row);
        double y = fDetectorPlane -> GetY(layer, row);

        // if(fDetector -> IsDeadChan(0, padID)){continue;}
        auto adcArr = fChannel -> GetWaveformY();

        double adcPoint = -999;
        double tbPoint = -999;
        if(fMethodOpt == 0){
            GetMaxPoint(adcArr, adcPoint, tbPoint);
        }
        else{
            // to be updated for other method
        }

        if(adcPoint < fThreshold){continue;}

        fHit = (LKHit*)fHitArray -> ConstructedAt(hitNum);
        fHit -> SetPadID(padID);
        fHit -> SetLayer(layer);
        fHit -> SetRow(row);
        fHit -> SetX(x);
        fHit -> SetY(y);
        fHit -> SetTb(tbPoint);
        fHit -> SetW(adcPoint);

        hitNum++;
    }
    

    lk_info << " number of Hit: " << fHitArray -> GetEntries() << endl;  
}

bool STDPulseAnalyser::EndOfRun()
{
    return true;
}

void STDPulseAnalyser::SetMethod(int opt){fMethodOpt = opt;}

void STDPulseAnalyser::GetMaxPoint(int* adcArray, double& maxADC, double& maxTB)
{
    for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
        double adc = double(adcArray[tb]);
        if(maxADC < adc){
            maxADC = double(adc);
            maxTB = double(tb+0.5);
        }
    }
}
