#ifndef STDChannelViewer_HH
#define STDChannelViewer_HH

#include "LKRun.h"
#include "LKTask.h"
#include "LKContainer.h"
#include "LKDetectorSystem.h"
#include "LKDetector.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"
#include "LKHit.h"
#include "TClonesArray.h"

#include "TFile.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TH2Poly.h"
#include "TGraph.h"

class STDChannelViewer : public LKTask
{ 
    public:
        STDChannelViewer();
        virtual ~STDChannelViewer() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

        bool OnEventFigure(){fIsOnEventFigure = true;}

        void SetRunNumber(TString run){fRunNum = run;}

    private:
        TPCDrum *fDetector = nullptr;
        STDPadPlane *fPadPlane = nullptr;

        TClonesArray* fChannelArray = nullptr;
        GETChannel* fChannel = nullptr;

        // test
        bool fIsOnEventFigure;
        TCanvas* cEvent = nullptr;
        TH2Poly* hPoly = nullptr;
        TH2Poly* hBoundary = nullptr;

        TH1D* hHitNum = nullptr;
        TH1D* hSumADC = nullptr;
        TH2D* hChannel = nullptr;

        TString fRunNum;


    ClassDef(STDChannelViewer, 1)
};

#endif