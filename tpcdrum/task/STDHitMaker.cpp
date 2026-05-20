#include "STDHitMaker.h"

ClassImp(STDHitMaker);

STDHitMaker::STDHitMaker()
{
    fName = "STDHitMaker";
}

bool STDHitMaker::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fDetectorPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();
    fChannelArray = fRun -> GetBranchA("RawPad");

    fPulseAnalyzer = new STDPulseAnalyzer();
    fPulseAnalyzer -> Init();

    return true;
}

void STDHitMaker::Exec(Option_t *option)
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

        fPulseAnalyzer -> SetNextChannel(fChannel);
        auto channelHit = fPulseAnalyzer -> GetChannelHit();

        for(int p=0; p<channelHit.HitNum; p++){
            double adc = channelHit.GetADC(p);
            double tb = channelHit.GetTB(p);

            fHit = (LKHit*)fHitArray -> ConstructedAt(hitNum);
            fHit -> SetPadID(padID);
            fHit -> SetLayer(layer);
            fHit -> SetRow(row);
            fHit -> SetX(x);
            fHit -> SetY(y);
            fHit -> SetTb(tb);
            fHit -> SetW(adc);

            hitNum++;
        }
    }
    lk_info << " number of Hit: " << fHitArray -> GetEntries() << endl;  
}

bool STDHitMaker::EndOfRun()
{
    return true;
}

