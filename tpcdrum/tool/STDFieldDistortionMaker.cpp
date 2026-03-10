#include "STDFieldDistortionMaker.h"

STDFieldDistortionMaker::STDFieldDistortionMaker() : fInitEFieldFlag(false), fInitBFieldFlag(false)
{
}

bool STDFieldDistortionMaker::Init()
{
    fRun = LKRun::GetRun();
    fPar = fRun -> GetParameterContainer();

    bool onFieldDistortion = false;
    if(fPar->CheckPar("TPCDrum/FieldDistortion")){
        onFieldDistortion = fPar->GetParBool("TPCDrum/FieldDistortion");
    }
    if(onFieldDistortion){
        InitEFieldMap();
        InitBFieldMap();
    }

    return true;
}

double STDFieldDistortionMaker::GetEField(double x, double y, double z, int direction)
{
    double fieldValue = 0.;  // [V/cm]

    if(!fInitEFieldFlag){
        if(direction == dirX){fieldValue = 0;}
        else if(direction == dirY){fieldValue = 0;}
        else if(direction == dirZ){fieldValue = 250.;}
        else{fieldValue = -1;}
    }
    else{
        if(direction == dirX){fieldValue = mEFieldMap[0] -> Interpolate(x, y, z);}
        else if(direction == dirY){fieldValue = mEFieldMap[1] -> Interpolate(x, y, z);}
        else if(direction == dirZ){fieldValue = mEFieldMap[2] -> Interpolate(x, y, z);}
        else{fieldValue = -999;}
    }
    return fieldValue;
}

double STDFieldDistortionMaker::GetEFieldMag(double x, double y, double z)
{
    double ex = GetEField(x, y, z, dirX);
    double ey = GetEField(x, y, z, dirY);
    double ez = GetEField(x, y, z, dirZ);
    double e = sqrt(ex*ex + ey*ey + ez*ez);
    return e;
}

double STDFieldDistortionMaker::GetBField(double x, double y, double z, int direction)
{
    double fieldValue = 0.;  // [T]

    if(!fInitBFieldFlag){
        if(direction == dirX){fieldValue = 0;}
        else if(direction == dirY){fieldValue = 0;}
        else if(direction == dirZ){fieldValue = 1.;}
        else{fieldValue = -999;} // wrong direction !!
    }
    else{
        // To be updated the B-field map getting function
    }

    return fieldValue;
}

double STDFieldDistortionMaker::GetBFieldMag(double x, double y, double z)
{
    double bx = GetBField(x, y, z, dirX);
    double by = GetBField(x, y, z, dirY);
    double bz = GetBField(x, y, z, dirZ);
    double b = sqrt(bx*bx + by*by + bz*bz);
    return b;
}

bool STDFieldDistortionMaker::GetElectronDirection(double x, double y, double z, double& dx, double& dy, double& dz)
{
    double ex = GetEField(x, y, z, dirX);
    double ey = GetEField(x, y, z, dirY);
    double ez = GetEField(x, y, z, dirZ);
    double e  = GetEFieldMag(x, y, z);
    if(e <= 0.){return false;}

    dx = ex/e;
    dy = ey/e;
    dz = -1.*ez/e;
    return true;
}

void STDFieldDistortionMaker::InitEFieldMap()
{
    // To be updated 
    if(fPar->CheckPar("TPCDrum/FieldDistortion/EFieldDataPath")){
        TString dataPath = fPar->GetParString("TPCDrum/FieldDistortion/EFieldDataPath");
        
        TFile* file = new TFile(dataPath, "READ");
        mEFieldMap[0] = (TH3D*)file -> Get("EFieldMap_ex");
        mEFieldMap[1] = (TH3D*)file -> Get("EFieldMap_ey");
        mEFieldMap[2] = (TH3D*)file -> Get("EFieldMap_ez");
        mEFieldMap[3] = (TH3D*)file -> Get("EFieldMap_v");
        file -> Close();

        fInitEFieldFlag = true;
        cout << "STDFieldDistortionMaker::InitEFieldMap() -- E-Field map has been initialized" << endl;
    }
    else{
        cout << "STDFieldDistortionMaker::InitEFieldMap() -- TPCDrum/FieldDistortion/EFieldDataPath is wrong, turn off the E-Field map" << endl;
    }
}

void STDFieldDistortionMaker::InitBFieldMap()
{
    if(fPar->CheckPar("TPCDrum/FieldDistortion/BFieldDataPath")){
        TString dataPath = fPar->GetParString("TPCDrum/FieldDistortion/BFieldDataPath");

        fInitBFieldFlag = true;
    }
    else{
        cout << "STDFieldDistortionMaker::InitBFieldMap() -- TPCDrum/FieldDistortion/BFieldDataPath is wrong, turn off the E-Field map" << endl;
    }
}