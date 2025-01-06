#ifndef STDChannelViewer_HH
#define STDChannelViewer_HH

#include "LKRun.h"
#include "LKTask.h"
#include "LKContainer.h"
#include "LKDetectorSystem.h"
#include "LKDetector.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"
#include "LKPad.h"
#include "TClonesArray.h"

#include "TH1D.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TH2Poly.h"

class STDChannelViewer : public LKTask
{ 
    public:
        STDChannelViewer();
        virtual ~STDChannelViewer() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:

        TPCDrum *fDetector = nullptr;
        STDPadPlane *fPadPlane = nullptr;

        TClonesArray* fPadArray = nullptr;
        LKPad* fPad = nullptr;

        // All channel raw ADC viewer 
        TCanvas* cChannel = nullptr;
        TH2I* hChanADC[4][68]; // FPN subtracted ADC

        // All hits by events into Pad Poly
        TCanvas* cEvent = nullptr;
        TH2Poly* hPoly = nullptr;   


    ClassDef(STDChannelViewer, 1)
};

#endif