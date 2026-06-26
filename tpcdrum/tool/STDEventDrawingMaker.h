#ifndef STDEventDrawingMaker_HH
#define STDEventDrawingMaker_HH

#include "LKRun.h"
#include "LKParameterContainer.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"
#include "STDSiArray.h"

#include "TCanvas.h"
#include "TLatex.h"
#include "TH1D.h"
#include "TH2Poly.h"
#include "TGraph.h"

class STDEventDrawingMaker
{
    public:
        STDEventDrawingMaker();
        virtual ~STDEventDrawingMaker(){}

        bool Init();

        void SetFigurePath(TString path){fFigurePath = path;}

        void FillTPC(int padID, double adc, double tb);
        void FillSi(int siDetID, int unitPadID, double adc);

        void SaveEventFigure();

        TCanvas* GetCanvas(){return fEventCanvas;}

    private:
        void InitHist();

        LKRun* fRun;
        LKParameterContainer* fPar;
        TPCDrum* fDetector;
        STDPadPlane* fPadPlane;
        STDSiArray* fSiArray;

        TString fFigurePath;
        bool fIsMC;

        TCanvas* fEventCanvas;
        TLatex* fLatex;
        TH2Poly* fBasePoly[2];
        TH2Poly* fPadPoly;
        TH2Poly* fSiPoly[8];
};

#endif