#include "STDSimTuningManager.h"

STDSimTuningManager* STDSimTuningManager::fInstance = nullptr;

STDSimTuningManager* STDSimTuningManager::GetSimTuningManager()
{
    if (fInstance != nullptr){return fInstance;}
    return new STDSimTuningManager();
}

STDSimTuningManager::STDSimTuningManager()
{
    fInstance = this;
    Init();
}

bool STDSimTuningManager::Init()
{
    cout << "STDSimTuningManager::Init()" << endl;

    fRun = LKRun::GetRun();
    fPar = fRun -> GetParameterContainer();

    InitGarfieldGasData();
    InitGEMGain();
    InitPulseShape();

    fElectronStepSize = 1.; // [mm]
    if(fPar->CheckPar("TPCDrum/ElectronStepSize")){
        fElectronStepSize = fPar->GetParDouble("TPCDrum/ElectronStepSize");
    }

    return true;
}

double STDSimTuningManager::GetWValue(){return 41.;} // test !!! 

double STDSimTuningManager::GetDriftVelocity(double x, double y, double z)
{
    double efield = fFieldDistortion -> GetEFieldMag(x, y, z);
    // double bfield = fFieldDistortion -> GetBFieldMag(x, y, z); // to be updated

    if(fIsInitGarfieldData){
        return mVelocityData -> Eval(efield); // [mm/ns] 
    }
    return 0.9 * 0.01; // [mm/ns] 
}

double STDSimTuningManager::GetDiffusionT(double x, double y, double z)
{
    double efield = fFieldDistortion -> GetEFieldMag(x, y, z);
    // double bfield = fFieldDistortion -> GetBFieldMag(x, y, z); // to be updated

    if(fIsInitGarfieldData){
        return mTransDiffusionData -> Eval(efield); // [mm/sqrt(mm)]
    }
    return 0.02 * 10./sqrt(10.);// [mm/sqrt(mm)]
}

double STDSimTuningManager::GetDiffusionL(double x, double y, double z)
{
    double efield = fFieldDistortion -> GetEFieldMag(x, y, z);
    // double bfield = fFieldDistortion -> GetBFieldMag(x, y, z); // to be updated

    if(fIsInitGarfieldData){
        return mLongiDiffusionData -> Eval(efield); // [mm/sqrt(mm)]
    }
    return 0.018 * 10./sqrt(10.); // [mm/sqrt(mm)]
}

double STDSimTuningManager::GetGEMGainFactor(double x, double y, double z)
{
    double gainRatio = fGainCalibration -> GetGEMGainRatio(x, y, z);
    return gainRatio*GetGEMGain();
}

double STDSimTuningManager::GetGEMDiffusionT()
{
    return 0.02 * 10./sqrt(10.);// [mm/sqrt{mm}] test !!!
}

double STDSimTuningManager::GetGEMDiffusionL()
{
    return 0.018 * 10./sqrt(10.); // [mm/sqrt{mm}] test !!!
}

double STDSimTuningManager::GetGEMDriftVelocity()
{
    return 100.; // [mm/ns] test !!!
}

double STDSimTuningManager::GetExtraDiffusionT()
{
    return 0.;
}

void STDSimTuningManager::InitGarfieldGasData()
{
    fIsInitGarfieldData = false;
    if(fPar->CheckPar("TPCDrum/GarfieldGasOn")){
        fIsInitGarfieldData = fPar->GetParBool("TPCDrum/GarfieldGasOn");
    }
    cout << fIsInitGarfieldData << endl;

    TString dataPath = "";
    if(fPar->CheckPar("TPCDrum/SimDataPath")){
        dataPath = fPar->GetParString("TPCDrum/SimDataPath");
        if(dataPath[dataPath.Sizeof()-1] != '/'){dataPath += "/";}
        dataPath += fPar->GetParString("TPCDrum/STDGarfieldData");
    }

    cout << " dataPath " << dataPath << endl;

    if(dataPath != "" && fIsInitGarfieldData){

        TFile* file = new TFile(dataPath, "READ");
        mVelocityData = (TGraph*)file -> Get("DriftVelocity");
        mTransDiffusionData = (TGraph*)file -> Get("TranseverseDiffusion");
        mLongiDiffusionData = (TGraph*)file -> Get("LongitudinalDiffusion");

        if(mVelocityData == nullptr){
            fIsInitGarfieldData = false;
            cout << "STDSimTuningManager::InitGarfieldGasData() -- No Initialized STDGarfield data.." << endl;
        }
        else{
            cout << "STDSimTuningManager::InitGarfieldGasData() -- STDGarfield has been nitialized " << dataPath << endl;
        }
    }
    else{
        cout << "STDSimTuningManager::InitGarfieldGasData() -- No STDGarfield data, used default parameters" << endl;
    }
}

void STDSimTuningManager::InitGEMGain()
{
    // Gain fluctuation distribution based on Polya distribution
    fGEMGainDist = new TF1("function", this, &STDSimTuningManager::PolyaDistribution, 0., 50000., 2);
    fGEMGainDist -> SetParameter(0, 1.5); // M, See the STAR TPC gain fluctuation
    fGEMGainDist -> SetParameter(1, 6500); // Intrincsic gain
}


void STDSimTuningManager::InitPulseShape()
{
    bool onPulseShape = false;
    if(fPar->CheckPar("TPCDrum/PulseShapeOn")){
        onPulseShape = fPar->GetParBool("TPCDrum/PulseShapeOn");
    }
    TString dataPath = "";
    if(fPar->CheckPar("TPCDrum/SimDataPath")){
        dataPath = fPar->GetParString("TPCDrum/SimDataPath");
        if(dataPath[dataPath.Sizeof()-1] != '/'){dataPath += "/";}
        dataPath += fPar->GetParString("TPCDrum/PulseShapeData");
    }
    if(dataPath != "" && onPulseShape){
        
    }
    else{
        cout << "STDSimTuningManager::InitPulseShape() -- No Pulse shape data, not make the pulse" << endl;
    }
}

void STDSimTuningManager::InitNoiseShape()
{
    bool onNoiseShape = false;
    if(fPar->CheckPar("TPCDrum/NoiseShapeOn")){
        onNoiseShape = fPar->GetParBool("TPCDrum/NoiseShapeOn");
    }
    TString dataPath = "";
    if(fPar->CheckPar("TPCDrum/SimDataPath")){
        dataPath = fPar->GetParString("TPCDrum/SimDataPath");
        if(dataPath[dataPath.Sizeof()-1] != '/'){dataPath += "/";}
        dataPath += fPar->GetParString("TPCDrum/NoiseShapeData");
    }
    if(dataPath != "" && onNoiseShape){

    }
    else{
        cout << "STDSimTuningManager::InitPulseShape() -- No Noise shape data, not make the noise" << endl;
    } 
}

double STDSimTuningManager::GetGEMGain()
{
    double gain = fGEMGainDist -> GetRandom(); // test !!
    return gain;
}

Double_t STDSimTuningManager::PolyaDistribution(Double_t* x, Double_t* par)
{
    Double_t value = par[0]*pow(par[0]*(x[0]/par[1]), par[0]-1.)/TMath::Gamma(par[0]) * exp(-1. * par[0]*(x[0]/par[1]));
    return value;
}

