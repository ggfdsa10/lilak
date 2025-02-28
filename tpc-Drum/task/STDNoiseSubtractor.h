#ifndef STDNoiseSubtractor_HH
#define STDNoiseSubtractor_HH

#include "LKRun.h"
#include "LKTask.h"
#include "LKContainer.h"
#include "LKDetectorSystem.h"
#include "LKDetector.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"
#include "GETChannel.h"
#include "TClonesArray.h"

#include "TH1D.h"
#include "TProfile.h"

class STDNoiseSubtractor : public LKTask
{ 
    public:
        STDNoiseSubtractor();
        virtual ~STDNoiseSubtractor() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:
        TPCDrum *fDetector = nullptr;
        STDPadPlane *fDetectorPlane = nullptr;

        TClonesArray* fChannelArray = nullptr;
        GETChannel* fChannel = nullptr;

        TH1D* fADCTmp = nullptr;
        TH2D* fADCTemplate[4];
        TProfile* fNoiseTemplate[4];

        int fAsAdNum = 4;
        int fTBStartIdx = 5;
        int fTBEndIdx = 508;
        int fTBIdxNum = 25; // for remove the pulse-like-pad
        int fTBInterval = 20; // for remove the pulse-like-pad

        double fTmpADCOffset = 1000.;

    ClassDef(STDNoiseSubtractor, 1)
};

#endif