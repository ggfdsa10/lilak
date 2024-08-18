#ifndef STDPulseAnalyser_HH
#define STDPulseAnalyser_HH

#include "LKRun.h"
#include "LKTask.h"
#include "LKContainer.h"
#include "LKDetectorSystem.h"
#include "LKDetector.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"
#include "LKPad.h"
#include "LKHit.h"
#include "TClonesArray.h"

class STDPulseAnalyser : public LKTask
{ 
    public:
        STDPulseAnalyser();
        virtual ~STDPulseAnalyser() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

        void SetMethod(int opt);

    private:
        void GetMaxPoint(int* adcArray, double& maxADC, double& maxTB);

        TPCDrum *fDetector = nullptr;
        STDPadPlane *fPadPlane = nullptr;

        TClonesArray* fPadArray = nullptr;
        LKPad* fPad = nullptr;

        TClonesArray* fHitArray = nullptr;
        LKHit* fHit = nullptr;

        int fMethodOpt; // [0 == Maximum Point, to be updated]
        double fThreshold = 40.;
        int fTBStartIdx = 100;
        int fTBEndIdx = 400;

    ClassDef(STDPulseAnalyser, 1)
};

#endif