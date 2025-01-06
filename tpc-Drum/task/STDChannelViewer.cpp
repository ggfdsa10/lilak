#include "STDChannelViewer.h"

ClassImp(STDChannelViewer);

STDChannelViewer::STDChannelViewer()
{
    fName = "STDChannelViewer";
}

bool STDChannelViewer::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();
    fPadArray = fRun -> GetBranchA("RawPad");

    hPoly = fPadPlane -> GetPadPlanePoly();
    for(int aget=0; aget<4; aget++){
        for(int chan=0; chan<68; chan++){
            hChanADC[aget][chan] = new TH2I(Form("chanADC_a%i_c%i", aget, chan), "", 500, 0, 500, 4000, -500, 3500);
            hChanADC[aget][chan] -> SetStats(0);
            hChanADC[aget][chan] -> SetTitle(Form("FPN subtracted raw ADC (AGET %i, Chan %i); TB; ADC", aget, chan));
        }
    }

    return true;
}

void STDChannelViewer::Exec(Option_t *option)
{
    const int padNum = fPadArray -> GetEntries();
    for(int pad=0; pad<padNum; pad++){
        fPad = (LKPad*)fPadArray -> At(pad);
        auto rawADCArr = fPad -> GetArrayRawSig();

        int asadID = fPad -> GetAsad();
        int agetID = fPad -> GetAget();
        int chanID = fPad -> GetChan();
        int padID = fPad -> GetPadID();
        double x = fPad -> GetX();
        double y = fPad -> GetY();

        int maxADC = 0;
        for(int tb=1; tb<500; tb++){
            int adc = rawADCArr[tb];

            hChanADC[agetID][chanID] -> Fill(tb+1, adc);

            if(adc > maxADC){
                maxADC = adc;
            }
        }
        if(maxADC > 150){
            hPoly -> Fill(x, y);
        }
    }

}

bool STDChannelViewer::EndOfRun()
{

    cEvent = new TCanvas("polyCanvas"," polyCanvas");

    cEvent -> cd();
    hPoly -> SetTitle(";x [mm]; y [mm]");  
    gPad -> SetLogz();
    hPoly -> Draw("colz, text"); 
    cEvent -> Draw();
    cEvent -> SaveAs("./ChanTestPad.pdf");

    cChannel = new TCanvas("chanCanvas", "chanCanvas");

    for(int aget=0; aget<4; aget++){
        for(int chan=0; chan<68; chan++){
            cChannel -> cd();

            int entry = hChanADC[aget][chan] -> GetEntries();
            if(entry == 0){continue;}

            hChanADC[aget][chan] -> Draw("colz");

            cChannel -> Draw();
            cChannel -> SaveAs(Form("./chanViewer/deadChan_a%i_c%i.png", aget, chan));
        }
    }

    return true;
}