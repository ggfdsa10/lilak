#include "TPCDrum.h"
#include "STDPadPlane.h"

ClassImp(TPCDrum);

TPCDrum::TPCDrum()
{
    fName = "TPCDrum";
    if (fDetectorPlaneArray==nullptr)
        fDetectorPlaneArray = new TObjArray();
}

bool TPCDrum::Init()
{
    LKDetector::Init();

    lk_info << "Initializing TPCDrum" << std::endl;

    return true;
}

void TPCDrum::Print(Option_t *option) const
{
    lk_info << "TPCDrum" << std::endl;
}

bool TPCDrum::BuildGeometry()
{
    return true;
}

bool TPCDrum::BuildDetectorPlane()
{
    AddPlane(new STDPadPlane);
    return true;
}

bool TPCDrum::IsInBoundary(Double_t x, Double_t y, Double_t z)
{
    // example (x,y,z) is inside the plane boundary
    //if (x>-10 and x<10)
    //    return true;
    //return false;
    return true;
}


bool TPCDrum::IsDeadChan(int config, int padID)
{
    if(config == 0){
        return IsDeadChanForMainRun_OldZAP(padID);
    }
    return 0;
}

bool TPCDrum::IsDeadChanForMainRun_OldZAP(int padID)
{
    switch (padID)
    {
        case 25:
            return true;
        case 26:
            return true;
        case 27:
            return true;
        case 28:
            return true;
        case 29:
            return true;
        case 30:
            return true;
        case 31:
            return true;
        case 89:
            return true;
        case 90:
            return true;
        case 91:
            return true;
        case 92:
            return true;
        case 93:
            return true;
        case 94:
            return true;
        case 95:
            return true;
        case 154:
            return true;
        case 155:
            return true;
        case 156:
            return true;
        case 157:
            return true;
        case 158:
            return true;
        case 159:
            return true;
        // case 205:
        //     return true; // to be checked!!! not sufficiently statistics
        case 217:
            return true;
        case 218:
            return true;
        case 219:
            return true;
        case 220:
            return true;
        case 221:
            return true;
        case 222:
            return true;
        case 223:
            return true;

        default:
            return false;
    }
}