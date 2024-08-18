#ifndef STDEventViewer_HH
#define STDEventViewer_HH

#include "LKRun.h"
#include "LKTask.h"
#include "LKContainer.h"
#include "LKDetectorSystem.h"
#include "LKDetector.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"
#include "LKHit.h"
#include "TClonesArray.h"

#include "TH1D.h"
#include "TH1I.h"
#include "TH2Poly.h"
#include "TGraph.h"

class STDEventViewer : public LKTask
{ 
    public:
        STDEventViewer();
        virtual ~STDEventViewer() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:

        TPCDrum *fDetector = nullptr;
        STDPadPlane *fPadPlane = nullptr;

        TClonesArray* fHitArray = nullptr;
        LKHit* fHit = nullptr;

        // test
        TCanvas* cEvent = nullptr;
        TH2Poly* hPoly = nullptr;
        TGraph* hGraph = nullptr;

        TH1D* hSumADC = nullptr;
        TH1I* hHitNum = nullptr;



    ClassDef(STDEventViewer, 1)
};

#endif