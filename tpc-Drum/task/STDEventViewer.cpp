#include "STDEventViewer.h"

ClassImp(STDEventViewer);

STDEventViewer::STDEventViewer()
{
    fName = "STDEventViewer";
}

bool STDEventViewer::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();
    fHitArray = fRun -> GetBranchA("Hit");

    hPoly = fPadPlane -> GetPadPlanePoly();
    // hPoly -> GetZaxis() -> SetRangeUser(40, 3600);

    cEvent = new TCanvas();
    hSumADC = new TH1D("hSumADC","",40, 0, 5000);
    hHitNum = new TH1I("hHitNum","",30, 0, 30);
    hGraph = new TGraph();
    hGraph -> SetMarkerStyle(20);
    hGraph -> SetMarkerSize(1);
    hGraph -> SetMarkerColor(kRed);

    return true;
}

void STDEventViewer::Exec(Option_t *option)
{
    // hPoly -> ClearBinContents();
    hGraph -> Set(0);
    
    double maxW = 0.;
    int maxSection = -1;
    double maxX = 0.;
    double maxY = 0.;
    int maxHitIdx = -1;
    int hitNum = fHitArray -> GetEntries();
    for(int hit=0; hit<hitNum; hit++){
        fHit = (LKHit*)fHitArray -> At(hit);
        int padID = fHit -> GetPadID();
        int layer = fHit -> GetLayer();
        int row = fHit -> GetRow();
        double x = fHit -> X();
        double y = fHit -> Y();
        double w = fHit -> W();
        double tb = fHit -> GetTb();
        // hPoly -> Fill(x, y, w);

        if(maxW < w){
            maxW = w;
            maxHitIdx = hit;
            maxX = x;
            maxY = y;
            maxSection = fPadPlane -> GetSectionID(padID);
        }
    }
    if(maxW > 3400 || maxW < 100){return;}
    if(hitNum < 3 || hitNum > 6){return;}
    if(maxSection != 2){return;}

    double sumADC = 0.;

    fHit = (LKHit*)fHitArray -> At(maxHitIdx);
    int maxLayer = fHit -> GetLayer();
    int maxRow = fHit -> GetRow();
    int neighberHitNum = 0;
    sumADC += maxW;

    bool isCorrectSection = true;
    for(int hit=0; hit<hitNum; hit++){
        if(hit == maxHitIdx){continue;}
        fHit = (LKHit*)fHitArray -> At(hit);
        int padID = fHit -> GetPadID();
        int layer = fHit -> GetLayer();
        int row = fHit -> GetRow();
        int section = fPadPlane -> GetSectionID(padID);

        if(abs(layer - maxLayer) <= 0 && abs(row - maxRow) < 3){
            double w = fHit -> W();
            if(w < 100){continue;}
            sumADC += w;
            neighberHitNum++;

            if(section != 2){
                isCorrectSection = false;

                if(row == 36){
                    break;
                }
            }
        }
    }

    if(!isCorrectSection){return;}
    
    hPoly -> Fill(maxX, maxY);
    // for(int hit=0; hit<hitNum; hit++){
    //     fHit = (LKHit*)fHitArray -> At(hit);
    //     int padID = fHit -> GetPadID();
    //     double x = fHit -> X();
    //     double y = fHit -> Y();

    // }

    if(2 <= neighberHitNum){
        lk_info << "number of neighber Hit: " << neighberHitNum << " Sum of ADC: " << sumADC << endl;
        hSumADC -> Fill(sumADC);
    }
    hHitNum -> Fill(hitNum);
}

bool STDEventViewer::EndOfRun()
{
    TCanvas* c1 = new TCanvas("","",1200,1200);
    c1 -> Divide(2,2);

    c1 -> cd(1);
    hPoly -> Draw("colz");

    c1 -> cd(2);
    hHitNum -> Draw();

    c1 -> cd(3);
    hSumADC -> Draw();

    c1 -> Draw();
    c1 -> SaveAs("./eventTotal.pdf");

    return true;
}