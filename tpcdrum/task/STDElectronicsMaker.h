#ifndef STDElectronicsMaker_HH
#define STDElectronicsMaker_HH

#include <time.h>
#include <vector>
#include <tuple>

#include "LKRun.h"
#include "LKTask.h"
#include "LKParameterContainer.h"
#include "LKDetector.h"
#include "GETChannel.h"
#include "LKMCTag.h"
#include "LKMCTrack.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"
#include "STDSimTuningManager.h"
#include "STDPulseAnalyzer.h"

#include "TClonesArray.h"

class STDElectronicsMaker : public LKTask
{ 
    public:
        STDElectronicsMaker();
        virtual ~STDElectronicsMaker() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:
        TPCDrum *fDetector;
        STDPadPlane *fPadPlane;

        STDSimTuningManager* fTuneManager;
        STDPulseAnalyzer* fPulseAnalyzer;

        TClonesArray* fChannelArray;
        TClonesArray* fMCTagArray;
        TClonesArray* fTrackArray;

        GETChannel* fChannel;
        LKMCTag* fMCTag;
        LKMCTrack* fTrack;

        Double_t fDynamicRange;
        Double_t fEChargeToADC;
        const Double_t fElectronCharge = 1.6021773349e-19; // [C]
        const Double_t fADCMaxAmp = 4096; // maximum ADC

    ClassDef(STDElectronicsMaker, 1)
};

#endif