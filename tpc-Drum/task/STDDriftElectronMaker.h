#ifndef STDDriftElectronMaker_HH
#define STDDriftElectronMaker_HH

#include "LKRun.h"
#include "LKTask.h"
#include "LKParameterContainer.h"
#include "LKDetectorSystem.h"
#include "LKDetector.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"
#include "TClonesArray.h"

class STDDriftElectronMaker : public LKTask
{ 

    public:
        STDDriftElectronMaker();
        virtual ~STDDriftElectronMaker() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:
        void MakeElectronCluster();
        void DriftElectron();
        void AvalancheElectron();

        LKParameterContainer* fPar;
        TPCDrum *fDetector;
        STDPadPlane *fPadPlane;

        TClonesArray* fChannelArray;
        TClonesArray* fTrackArray;
        TClonesArray* fStepArray;

    ClassDef(STDDriftElectronMaker, 1)
};

#endif