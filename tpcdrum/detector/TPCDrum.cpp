#include "TPCDrum.h"

ClassImp(TPCDrum);

TPCDrum::TPCDrum()
{
    fName = "TPCDrum";
    if (fDetectorPlaneArray==nullptr)
        fDetectorPlaneArray = new TObjArray();


    fPadPlane = new STDPadPlane();
}

bool TPCDrum::Init()
{
    lk_info << "Initializing TPCDrum" << std::endl;
    LKDetector::Init();


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
    AddPlane(new STDSiArray);
    return true;
}

bool TPCDrum::IsInBoundary(Double_t x, Double_t y, Double_t z)
{
    double activeHeight = fFieldCageHeight + fFieldCageSpacing + 3.*fGEMSpacing;
    if(0 <= z && z <= activeHeight){
        if(GetDetectorPlane(0)->IsInBoundary(x, y)){
            return true;
        }
    }

    return false;
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

void TPCDrum::GetCoordinateGeantToPad(double& x, double& y, double& z)
{
    double tmpX = x;
    double tmpY = y;
    double tmpZ = z;

    // Convert Z and Y axis
    tmpZ = tmpY;
    tmpY = z;

    // Shift drift height distance
    double activeDriftHeight = fChamberHeight/2. - fReadoutPCBThickness;
    tmpZ = tmpZ + activeDriftHeight;

    // Shift y-axis 
    double padHeight = fPadPlane->GetPadHeight();
    double padGap = fPadPlane->GetPadGap();
    double layerNum = fPadPlane->GetLayerNum();
    double shiftHeight = (padHeight+padGap)*(layerNum/2.-1.) + (padHeight+padGap)/2.;
    tmpY = tmpY + shiftHeight;

    x = -1.*tmpX;
    y = tmpY;
    z = tmpZ;
}

void TPCDrum::GetCoordinatePadToGeant(double& x, double& y, double& z)
{
    double tmpX = x;
    double tmpY = y;
    double tmpZ = z;

    // Shift y-axis 
    double padHeight = fPadPlane->GetPadHeight();
    double padGap = fPadPlane->GetPadGap();
    double layerNum = fPadPlane->GetLayerNum();
    double shiftHeight = (padHeight+padGap)*(layerNum/2.-1.) + (padHeight+padGap)/2.;
    tmpY = tmpY - shiftHeight;

    // Shift drift height distance
    double activeDriftHeight = fChamberHeight/2. - fReadoutPCBThickness;
    tmpZ = tmpZ - activeDriftHeight;

    // Convert Z and Y axis
    double tmpY2 = tmpY;
    tmpY = tmpZ;
    tmpZ = tmpY2;

    x = -1.*tmpX;
    y = tmpY;
    z = tmpZ;
}

double TPCDrum::GetFieldCageGeantYPos()
{
    double height = fChamberHeight/2. - fFieldCageHeight/2. - fReadoutPCBThickness - 3.*fGEMSpacing - fFieldCageSpacing;
    return height;
}

double TPCDrum::GetGatingGridGeantYPos()
{
    double fieldCageHeight = GetFieldCageGeantYPos();
    double fieldCageCenterToBeamWindow = fFieldCageUpstreamWindowHeight - fFieldCageUpstreamWindowSize/2. - fFieldCageHeight/2.;
    double height = fieldCageHeight - fieldCageCenterToBeamWindow + fPCBThickness/2.;
    return height;
}
