#ifndef STDSiResponseMaker_HH
#define STDSiResponseMaker_HH

#include "LKRun.h"
#include "LKTask.h"
#include "LKParameterContainer.h"
#include "LKDetector.h"
#include "LKMCStep.h"
#include "GETChannel.h"
#include "LKMCTag.h"

#include "TRandom3.h"
#include "TClonesArray.h"

#include "TPCDrum.h"
#include "STDSiArray.h"
#include "STDPadPlane.h"

#include "STDSimTuningManager.h"

class STDSiResponseMaker : public LKTask
{ 
    public:
        STDSiResponseMaker();
        virtual ~STDSiResponseMaker() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:
        void ConvertCoordinateGeantToPad();
        void MakeOhmicResponse(double& e);
        void MakeJunctionResponse(int siDetID, double x, double z, double e, double& junc1Energy, double& junc2Energy);

        TPCDrum* fDetector;
        STDSiArray* fSiArray;
        STDPadPlane* fPadPlane;

        STDSimTuningManager* fTuneManager;

        TRandom3* fRandom;

        TClonesArray* fChannelArray;
        TClonesArray* fMCTagArray;
        TClonesArray* fTrackArray;
        vector<TClonesArray*> fStepArray;

        bool fIsNewChannelArray;

        GETChannel* fChannel;
        LKMCTag* fMCTag;
        LKMCStep* fStep;

        double fTBTime;
        double fPulseDelay;

    ClassDef(STDSiResponseMaker, 1)
};

#endif