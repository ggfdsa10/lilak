#include "STDChannelViewer.h"

ClassImp(STDChannelViewer);

STDChannelViewer::STDChannelViewer()
: fIsOnEventFigure(false)
{
    fName = "STDChannelViewer";
}

bool STDChannelViewer::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();
    fChannelArray = fRun -> GetBranchA("RawPad");

    hPoly = fPadPlane -> GetPadPlanePoly();
    hPoly -> GetZaxis() -> SetRangeUser(1., 4000);
    hPolyTotal = (TH2Poly*)hPoly -> Clone("hPolyTotal");

    double x[4] = {-100., -100., 100., 100.};
    double y[4] = {-34., 166., 166., -34.};

    hBoundary = new TH2Poly();
    hBoundary -> SetStats(0);
    hBoundary -> SetTitle("TPC-Drum pad plane ; x [mm]; y [mm]");
    hBoundary -> AddBin(4, x, y);

    cEvent = new TCanvas("cEvent", "", 1200, 600.);
    cEvent -> Divide(2,1);
    

    cTatex = new TLatex();
    cTatex ->SetTextSize(0.04);

    hChannel = new TH2D("hChannel","",512, 0, 512, 3800, -200., 3600.);
    hChannel -> SetTitle("All channel pulse; TB;ADC");
    hChannel -> SetStats(0);

    hHitNum = new TH1D("hHitNum","", 100, 0, 100);
    hHitNum -> SetTitle("Hit (ADC>250) distribution; Hit; Counts");
    hHitNum -> SetStats(0);

    hSumADC = new TH1D("hHitNum","", 100, 0, 60000);
    hSumADC -> SetTitle("Sum of ADC distribution; Sum ADC; Counts");
    hSumADC -> SetStats(0);

    return true;
}

void STDChannelViewer::Exec(Option_t *option)
{
    hChannel -> Reset("ICESM");
    hPoly -> ClearBinContents();

    int hitNum = 0;
    double sumADC = 0.;

    const int channelNum = fChannelArray -> GetEntries();
    for(int chan=0; chan<channelNum; chan++){
        fChannel = (GETChannel*)fChannelArray -> At(chan);
        auto rawADCArr = fChannel -> GetWaveformY();

        int asadID = fChannel -> GetAsad();
        int agetID = fChannel -> GetAget();
        int chanID = fChannel -> GetChan();
        if(chanID == -1){continue;}
        int padID = fPadPlane -> GetPadID(asadID, agetID, chanID);

        int maxADC = 0;
        int maxTBIdx = 0;
        for(int tb=10; tb<500; tb++){
            double adc = rawADCArr[tb];
            if(maxADC < adc){
                maxADC = adc;
                maxTBIdx = tb;
            }
            hChannel -> Fill(tb+1, adc);
        }
        if(maxADC <= 50.){continue;}

        double x = fPadPlane -> GetX(padID);
        double y = fPadPlane -> GetY(padID);

        hPoly -> Fill(x, y, maxADC);
        hPolyTotal -> Fill(x, y, 1.);
        hitNum++;
        sumADC += maxADC;
    }

    hHitNum -> Fill(hitNum);
    hSumADC -> Fill(sumADC);

    if(fIsOnEventFigure && hitNum > 0){
        cEvent -> cd(1);
        hBoundary -> Draw("");
        hPoly -> Draw("colz, same");
        cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));
        cTatex -> DrawLatexNDC(0.13, 0.805, Form("Event %i", int(fRun -> GetCurrentEventID())));

        cEvent -> cd(2);
        hChannel -> Draw("colz");
        cTatex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));
        cTatex -> DrawLatexNDC(0.13, 0.805, Form("Event %i", int(fRun -> GetCurrentEventID())));

        cEvent -> Update();
        cEvent -> SaveAs(Form("./Run%s_Event%i.pdf", fRunNum.Data(), int(fRun -> GetCurrentEventID()) ));
    }
}

bool STDChannelViewer::EndOfRun()
{
    TCanvas* c1 = new TCanvas("","",1200,1200);
    c1 -> Divide(2,2);

    TLatex* latex = new TLatex();

    c1 -> cd(1);
    hHitNum -> GetYaxis()->SetRangeUser(0., hHitNum->GetMaximum()*1.5);
    hHitNum -> Draw();
    latex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));

    c1 -> cd(2);
    hSumADC -> GetYaxis()->SetRangeUser(0., hHitNum->GetMaximum()*1.5);
    hSumADC -> Draw();
    latex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));

    c1 -> cd(3);
    gPad -> SetLogz();
    hBoundary -> Draw("");
    hPolyTotal -> Draw("colz, same");
    latex -> DrawLatexNDC(0.13, 0.855, Form("Run %s", fRunNum.Data()));

    c1 -> Draw();
    c1 -> SaveAs(Form("./RunSummary_run%s.pdf", fRunNum.Data()));

    return true;
}