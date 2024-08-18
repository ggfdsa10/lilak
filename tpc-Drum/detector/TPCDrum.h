#ifndef TPCDRUM_HH
#define TPCDRUM_HH

#include "LKDetector.h"
#include "LKLogger.h"

class TPCDrum : public LKDetector
{
    public:
        TPCDrum();
        virtual ~TPCDrum() { ; }

        void Print(Option_t *option="") const;
        bool Init();
        bool BuildGeometry();
        bool BuildDetectorPlane();
        bool IsInBoundary(Double_t x, Double_t y, Double_t z);

        bool IsDeadChan(int conifg, int padID);

    private:
        bool IsDeadChanForMainRun_OldZAP(int padID); // test !!!

    ClassDef(TPCDrum,1);
};

#endif
