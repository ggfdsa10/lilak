#ifndef STDHitMaker_HH
#define STDHitMaker_HH

#include "LKRun.h"
#include "LKTask.h"
#include "LKContainer.h"
#include "LKDetectorSystem.h"
#include "LKDetector.h"

#include "TProfile.h"
#include "TSystem.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"
#include "GETChannel.h"
#include "LKHit.h"
#include "TClonesArray.h"

#include "STDPulseAnalyzer.h"

class STDHitMaker : public LKTask
{ 
    public:
        STDHitMaker();
        virtual ~STDHitMaker() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:
        TPCDrum *fDetector = nullptr;
        STDPadPlane *fDetectorPlane = nullptr;

        TClonesArray* fChannelArray = nullptr;
        GETChannel* fChannel = nullptr;

        TClonesArray* fHitArray = nullptr;
        LKHit* fHit = nullptr;

        STDPulseAnalyzer* fPulseAnalyzer;

    ClassDef(STDHitMaker, 1)
};

#endif