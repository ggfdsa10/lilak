#include "STDSiArray.h"

ClassImp(STDSiArray);

STDSiArray::STDSiArray()
{
    fName = "STDSiArray";
    if (fChannelArray==nullptr)
        fChannelArray = new TObjArray();
}

bool STDSiArray::Init()
{
    lk_info << "Initializing STDSiArray" << std::endl;
    
    InitSiArrayGeometry();
    InitChannelArray();

    return true;
}

void STDSiArray::Clear(Option_t *option)
{
    LKDetectorPlane::Clear(option);
}

void STDSiArray::Print(Option_t *option) const
{
    lk_info << "STDSiArray" << std::endl;
}

bool STDSiArray::IsInBoundary(Double_t x, Double_t z)
{

    return true;
}

Int_t STDSiArray::FindSiDetID(Double_t x, Double_t z)
{
    for(int i=0; i<fSiDetNum; i++){
        int unitPadID = fSiArrayPoly[i] -> FindBin(x+0.0001, z+0.0001);
        if(unitPadID > 0){return i;}
    }
    return -1;
}

Int_t STDSiArray::FindUnitPadID(Double_t x, Double_t z)
{
    for(int i=0; i<fSiDetNum; i++){
        int unitPadID = fSiArrayPoly[i] -> FindBin(x+0.0001, z+0.0001);
        if(unitPadID > 0){return unitPadID-1;}
    }
    return -1;
}

Int_t STDSiArray::GetStripIDFromPadID(int unitPadID)
{
    if(unitPadID < 0 || unitPadID >= fStripNum*fOhmicNum){return -1;}
    return int(unitPadID/4);
}

Int_t STDSiArray::GetStripIDFromJuncID(int juncID)
{
    if(juncID < 0 || juncID >= fJuncNum){return -1;}
    juncID = juncID+1;
    if(juncID%2==0){juncID -= 1;}
    int stripID = juncID/2;
    return stripID;
}

Int_t STDSiArray::GetOhmicID(int unitPadID)
{
    if(unitPadID < 0 || unitPadID >= fStripNum*fOhmicNum){return -1;}
    return int(unitPadID%4);
}

Int_t STDSiArray::GetSiDetID(int aget, int chan)
{
    int detID = -1;
    int chanIdx = chan - GetFPNCountIdx(chan);

    if(GetOhmicID(aget, chan) != -1){detID = chanIdx/fOhmicNum;}
    else if(GetJuncID(aget, chan) != -1){detID = chanIdx/fJuncNum + (aget-1)*4;}

    if(detID < 0 || detID >= fSiDetNum){return -1;}
    return detID;
}

Int_t STDSiArray::GetOhmicID(int aget, int chan)
{
    if(IsFPNChannel(chan)){return -1;}
    if(aget != 0){return -1;} // exclude junction channel aget
    if(chan > 33){return -1;} // end of 8th si detector

    int fpnCountIdx = GetFPNCountIdx(chan);
    int ohmicID = fOhmicNum - int((chan-fpnCountIdx)%4) -1; // starting from 0 index
    return ohmicID;
}

Int_t STDSiArray::GetJuncID(int aget, int chan)
{
    if(IsFPNChannel(chan)){return -1;}
    if(aget <= 0){return -1;} // exclude ohmic channel aget
    if(aget >= fAGETNum-1){return -1;} // end of 8th si detector

    int fpnCountIdx = GetFPNCountIdx(chan);
    int juncID = fJuncNum - int((chan-fpnCountIdx)%16) - 1;
    return juncID;
}

Int_t STDSiArray::GetStripID(int aget, int chan)
{
    int juncID = GetJuncID(aget, chan);
    return GetStripIDFromJuncID(juncID);
}

Double_t STDSiArray::GetX(int siDetID, int unitPadID){return fUnitPadPosMap_UnitPadIdx[siDetID].find(unitPadID)->second.first;}
Double_t STDSiArray::GetX(int siDetID, int strip, int ohmic){return fUnitPadPosMap_so[siDetID].find(make_pair(strip, ohmic))->second.first;}
Double_t STDSiArray::GetZ(int siDetID, int unitPadID){return fUnitPadPosMap_UnitPadIdx[siDetID].find(unitPadID)->second.second;}
Double_t STDSiArray::GetZ(int siDetID, int strip, int ohmic){return fUnitPadPosMap_so[siDetID].find(make_pair(strip, ohmic))->second.second;}

Int_t STDSiArray::GetFPNChannelID(int chan)
{
    if(0 <= chan && chan < 17) {return 11;} 
    else if(chan < 34){return 22;} 
    else if(chan < 51){return 45;} 
    else if(chan < 68){return 56;}
    return -1;
}

bool STDSiArray::IsFPNChannel(int chan)
{
    if(chan == 11 || chan == 22 || chan == 45 || chan == 56){return true;}
    return false;
}

Double_t STDSiArray::GetSiCenterPos(int detIdx, int xzIdx)
{
    if(detIdx < 0 || detIdx > fSiDetNum){return -999.;}
    if(xzIdx < 0 || xzIdx > 1){return -999.;}
    return fSiDetectorCenter[detIdx][xzIdx];
}

TString STDSiArray::GetSiName(int detIdx)
{
    if(detIdx < 0 || detIdx > fSiDetNum){return "";}
    return fSiDetectorName[detIdx];
}

TH2Poly* STDSiArray::GetSiPoly(int detIdx)
{
    if(detIdx < 0 || detIdx > fSiDetNum){return 0;}
    if(fSiArrayPoly[detIdx]){
        return fSiArrayPoly[detIdx];
    }
    return 0;
}

void STDSiArray::InitSiArrayGeometry()
{
    double beamCenterZ = 128.74; // [mm] distance from padplane to Si array beam window center
    double awayXBeamCenter = 25.6; // [mm] distance from beam window center to side-Si active area boundary
    double awayZBeamCenter = 20.7; // [mm] distance from beam window center to up/down-Si active area boundary
    double sideSiDist = 5.; // [mm] distance between side-Si
    double startHeight = 163.59 - fSiWidth/2.; // [mm] start height for side-Si

    double z = startHeight;
    for(int i=0; i<fSiDetNum-2; i++){
        if(i == 3){z = startHeight;}
        double sign = (i < 3)? +1. : -1.;

        double x = sign*(fSiHeight/2. +awayXBeamCenter);
        fSiDetectorCenter[i][0] = x;
        fSiDetectorCenter[i][1] = z;
        z -= (fSiWidth + sideSiDist);
    }

    for(int i=fSiDetNum-2; i<fSiDetNum; i++){
        double sign = (i%2==0)? +1. : -1.;
        double height = beamCenterZ + sign*(fSiHeight/2. +awayZBeamCenter);
        fSiDetectorCenter[i][0] = 0.;
        fSiDetectorCenter[i][1] = height;
    }

    fSiDetectorName[0] = "SiDet_RU";
    fSiDetectorName[1] = "SiDet_RM";
    fSiDetectorName[2] = "SiDet_RD";
    fSiDetectorName[3] = "SiDet_LU";
    fSiDetectorName[4] = "SiDet_LM";
    fSiDetectorName[5] = "SiDet_LD";
    fSiDetectorName[6] = "SiDet_U";
    fSiDetectorName[7] = "SiDet_D";

    for(int i=0; i<fSiDetNum; i++){
        fSiArrayPoly[i] = new TH2Poly();
        fSiArrayPoly[i] -> SetStats(0);
        fSiArrayPoly[i] -> SetTitle(";x [mm]; z [mm]");

        double siWidth = (i<6)? fSiHeight : fSiWidth;
        double siHeight = (i<6)? fSiWidth : fSiHeight;
        double siUnitPadWidth = (i<6)? fSiHeight/double(fOhmicNum) : fSiWidth/double(fStripNum);
        double siUnitPadHeight = (i<6)? fSiWidth/double(fStripNum) : fSiHeight/double(fOhmicNum);

        double centerPosX = fSiDetectorCenter[i][0];
        double centerPosZ = fSiDetectorCenter[i][1];

        double SiCoordSignX = (i < 3 || i==6)? +1. : -1.;
        double SiCoordSignZ = (i < 3 || i==7)? -1. : +1.;
        double SiSignX = (i < 3 || i==6)? -1. : +1.;
        double SiSignZ = (i < 3 || i==7)? +1. : -1.;

        int unitPadIdx = 0;
        for(int s=0; s<fStripNum; s++){
            for(int o=0; o<fOhmicNum; o++){
                // Si local coordinate defined at J1-O1 channel edge
                double siLocalX = siUnitPadWidth/2. + o * siUnitPadWidth;
                double siLocalZ = siUnitPadHeight/2. + s * siUnitPadHeight;
                if(i >= 6){
                    siLocalX = siUnitPadWidth/2. + s * siUnitPadWidth;
                    siLocalZ = siUnitPadHeight/2. + o * siUnitPadHeight;
                }

                // Convert coordinate from Si-local to padplane-local
                double siUnitPadCenterX = centerPosX +SiCoordSignX * siWidth/2. +SiSignX * siLocalX;
                double siUnitPadCenterZ = centerPosZ +SiCoordSignZ * siHeight/2. +SiSignZ * siLocalZ;

                fUnitPadPosMap_so[i].insert({make_pair(s, o), make_pair(siUnitPadCenterX, siUnitPadCenterZ)});
                fUnitPadPosMap_UnitPadIdx[i].insert({unitPadIdx, make_pair(siUnitPadCenterX, siUnitPadCenterZ)});

                // Make the boundary of unitSiPad
                double boundaryX[5];
                double boundaryZ[5];
                for(int b=0; b<5; b++){
                    double xSign = (b<2 || b==4)? -1. : +1.;
                    double zSign = (b==1 || b==2)? -1. : +1.;
        
                    boundaryX[b] = siUnitPadCenterX + xSign * siUnitPadWidth/2.;
                    boundaryZ[b] = siUnitPadCenterZ + zSign * siUnitPadHeight/2.;
                }
                fSiArrayPoly[i] -> AddBin(5, boundaryX, boundaryZ);
                unitPadIdx++;
            }
        }
    }
}

void STDSiArray::InitChannelArray()
{
    fChannelArray = new TObjArray();
    fMCTagArray = new TObjArray();

    for(int aget=0; aget<fAGETNum; aget++){
        for(int chan=0; chan<fChanNum; chan++){
            if(IsFPNChannel(chan)){continue;}

            GETChannel* channel = new GETChannel();
            channel -> SetAsad(3);
            channel -> SetAget(aget);
            channel -> SetChan(chan);
            fChannelArray -> Add(channel);

            LKMCTag* mcTag = new LKMCTag();
            fMCTagArray -> Add(mcTag);
        }
    }
}

int STDSiArray::GetFPNCountIdx(int chan)
{
    if(chan > 56){return 4;}
    else if(chan > 45){return 3;}
    else if(chan > 22){return 2;}
    else if(chan > 11){return 1;}
    return 0;
}