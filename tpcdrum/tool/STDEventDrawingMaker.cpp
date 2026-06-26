#include "STDEventDrawingMaker.h"

STDEventDrawingMaker::STDEventDrawingMaker()
: fFigurePath("./")
{
}

bool STDEventDrawingMaker::Init()
{
    fRun = LKRun::GetRun();
    fPar = fRun -> GetParameterContainer();
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane(0);
    fSiArray = (STDSiArray*) fDetector -> GetDetectorPlane(1);

    fIsMC = false;
    if(fPar -> CheckPar("TPCDrum/SimDataPath")){fIsMC = true;}

    InitHist();

    return true;
}

void STDEventDrawingMaker::FillTPC(int padID, double adc, double tb)
{
    // double binContent = fPadPoly -> GetBinContent(padID+1);
    // fPadPoly -> SetBinContent(padID+1, adc+binContent);
}

void STDEventDrawingMaker::FillSi(int siDetID, int unitPadID, double adc)
{
    double binContent = fSiPoly[siDetID] -> GetBinContent(unitPadID+1);
    fSiPoly[siDetID] -> SetBinContent(unitPadID+1, adc+binContent);
}

void STDEventDrawingMaker::SaveEventFigure()
{
    int eventNum = fRun -> GetCurrentEventID();

    fEventCanvas -> cd(3);
    fBasePoly[0] -> Draw("colz");
    // fPadPoly -> Draw("same, colz");

    if(fIsMC){fLatex -> DrawLatexNDC(0.12, 0.86, Form("TPCDrum simulation"));}
    else{fLatex -> DrawLatexNDC(0.12, 0.86, Form("Run %i", eventNum));}
    fLatex -> DrawLatexNDC(0.12, 0.81, Form("Event %i", eventNum));

    fEventCanvas -> cd(1);
    fBasePoly[1] -> Draw("colz");
    for(int si=0; si<fSiArray->GetSiNum(); si++){
        fSiPoly[si] -> Draw("same, colz");
    }
    if(fIsMC){fLatex -> DrawLatexNDC(0.12, 0.86, Form("TPCDrum simulation"));}
    else{fLatex -> DrawLatexNDC(0.12, 0.86, Form("Run %i", eventNum));}
    fLatex -> DrawLatexNDC(0.12, 0.81, Form("Event %i", eventNum));

    fEventCanvas -> Draw();
    fEventCanvas -> SaveAs(Form("%sEvent_%i.png", fFigurePath.Data(), eventNum));

    // fPadPoly -> ClearBinContents();
    for(int si=0; si<fSiArray->GetSiNum(); si++){
        fSiPoly[si] -> ClearBinContents();
    }
}

void STDEventDrawingMaker::InitHist()
{
    fEventCanvas = new TCanvas("events", "", 1200, 1200);
    fEventCanvas -> Divide(2,2);

    fLatex = new TLatex();
    fLatex -> SetTextSize(0.045);

    double x[4] = {-100., -100., 100., 100.};
    double y[4] = {-34., 166., 166., -34.};
    double z[4] = {0., 230., 0., 230.};

    fBasePoly[0] = new TH2Poly();
    fBasePoly[0] -> SetStats(0);
    fBasePoly[0] -> SetTitle("TPC PadPlane; x [mm]; y [mm]");
    fBasePoly[0] -> GetZaxis()->SetRangeUser(10., 4096.);
    fBasePoly[0] -> AddBin(4, x, y);

    fBasePoly[1] = new TH2Poly();
    fBasePoly[1] -> SetStats(0);
    fBasePoly[1] -> SetTitle("Si Array plane; x [mm]; z [mm]");
    fBasePoly[1] -> GetZaxis()->SetRangeUser(10., 4096.);
    fBasePoly[1] -> AddBin(4, x, z);

    TH2Poly* fPadPoly = fPadPlane -> GetPadPlanePoly();
    for(int si=0; si<fSiArray->GetSiNum(); si++){
        fSiPoly[si] = fSiArray -> GetSiPoly(si);
    }

}
