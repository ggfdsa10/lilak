#ifndef STDTest_HH
#define STDTest_HH

#include <time.h>

#include "LKRun.h"
#include "LKTask.h"
#include "LKParameterContainer.h"
#include "LKDetector.h"
#include "LKMCTrack.h"
#include "LKMCStep.h"
#include "GETChannel.h"
#include "LKMCTag.h"

#include "TMath.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TH2Poly.h"
#include "TH1D.h"

#include "TPCDrum.h"
#include "STDPadPlane.h"

class STDTest : public LKTask
{ 
    enum kPID{
        kNe20 = 0,
        kC12 = 1,
        kAlpha = 2,
        kB8 = 3
    };

    public:
        STDTest();
        virtual ~STDTest() {}

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:
        TPCDrum *fDetector;
        STDPadPlane *fPadPlane;

        TClonesArray* fMCTagArray;
        TClonesArray* fTrackArray;
        TClonesArray* fStepArray;

        LKMCTrack* fMCTrack;
        LKMCTag* fMCTag;
        LKMCStep* fStep;

        int id_ne20 = 1000100200;
        int id_c12 = 1000060120;
        int id_c12_decay = 1000060129;
        int id_alpha = 1000020040;
        int id_B8 = 1000040080;

        const double siPlaneY = 370.; // [mm]
        // const int 
        TH2D* hPlane[4];
        TH1D* hAlphaDistance;
        TH1D* hAlphaAngle;
        TH2D* hAlphaAngleDistance;
        TH1D* hTrkEnergy[4][2];

    ClassDef(STDTest, 1)
};

#endif