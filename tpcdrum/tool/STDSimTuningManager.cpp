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
    InitNoiseShape();

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
        if(efield <= 0.){return 0.;}
        return mVelocityData -> Eval(efield); // [mm/ns] 
    }
    return 0.9 * 0.01; // [mm/ns] 
}

double STDSimTuningManager::GetDiffusionT(double x, double y, double z)
{
    double efield = fFieldDistortion -> GetEFieldMag(x, y, z);
    // double bfield = fFieldDistortion -> GetBFieldMag(x, y, z); // to be updated

    if(fIsInitGarfieldData){
        if(efield <= 0.){return 0.;}
        return mTransDiffusionData -> Eval(efield); // [mm/sqrt(mm)]
    }
    return 0.02 * 10./sqrt(10.);// [mm/sqrt(mm)]
}

double STDSimTuningManager::GetDiffusionL(double x, double y, double z)
{
    double efield = fFieldDistortion -> GetEFieldMag(x, y, z);
    // double bfield = fFieldDistortion -> GetBFieldMag(x, y, z); // to be updated

    if(fIsInitGarfieldData){
        if(efield <= 0.){return 0.;}
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

void STDSimTuningManager::InitNoise()
{
    if(!fIsNoiseOn){return;}
    memset(fNoise, 0., sizeof(fNoise));

    // Generate the ASAD-shared noise to adding or abstracting the noise template from 2 to 5 times
    double weight = fRandom-> Uniform(0.5, 1.8)*100000.;
    for(int asad=0; asad<4; asad++){
        int mixNum = fRandom->Uniform(2, 5);
        for(int i=0; i<mixNum; i++){
            double sign = (fRandom -> Uniform(0, 2) == 0)? -1. : 1;
            fNoiseTree -> GetEntry(fRandom->Uniform(0, fNoiseEventNum));
            for(int tb=0; tb<512; tb++){
                fNoise[asad][tb] += (weight*sign*fNoiseTemplate[tb]);
            }
        }
    }
}

void STDSimTuningManager::AddChannelNoise(int asad, int* adc)
{
    if(!fIsNoiseOn){return;}
    if(asad >= 4){return;}

    // Generate the channel-by-channel noise with pedestal
    double pedestal = fRandom->Uniform(440, 550);
    double weight = fRandom -> Uniform(0.3, 0.7) *100000.;
    for(int i=0; i<2; i++){
        double sign = (fRandom -> Uniform(0, 2) == 0)? -1. : 1;
        fNoiseTree -> GetEntry(fRandom->Uniform(0, fNoiseEventNum));
        for(int tb=0; tb<512; tb++){
            if(i==0){adc[tb] = adc[tb] + fNoise[asad][tb] + pedestal;}
            adc[tb] = adc[tb] + weight*sign*fNoiseTemplate[tb];
            if(adc[tb] > 4096){adc[tb] = 4095;}
        }
    }
}

void STDSimTuningManager::InitGarfieldGasData()
{
    fIsInitGarfieldData = false;
    if(fPar->CheckPar("TPCDrum/GarfieldGasOn")){
        fIsInitGarfieldData = fPar->GetParBool("TPCDrum/GarfieldGasOn");
    }

    TString dataPath = "";
    if(fPar->CheckPar("TPCDrum/SimDataPath")){
        dataPath = fPar->GetParString("TPCDrum/SimDataPath");
        if(dataPath[dataPath.Sizeof()-1] != '/'){dataPath += "/";}
        dataPath += fPar->GetParString("TPCDrum/STDGarfieldData");
    }

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
            cout << "STDSimTuningManager::InitGarfieldGasData() -- STDGarfield has been initialized " << dataPath << endl;
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
    fGEMGainDist -> SetParameter(1, 2500); // Intrincsic gain
}

void STDSimTuningManager::InitNoiseShape()
{
    fIsNoiseOn = false;
    if(fPar->CheckPar("TPCDrum/NoiseShapeOn")){
        fIsNoiseOn = fPar->GetParBool("TPCDrum/NoiseShapeOn");
    }
    TString dataPath = "";
    if(fPar->CheckPar("TPCDrum/SimDataPath")){
        dataPath = fPar->GetParString("TPCDrum/SimDataPath");
        if(dataPath[dataPath.Sizeof()-1] != '/'){dataPath += "/";}
        dataPath += fPar->GetParString("TPCDrum/NoiseShapeData");
    }
    if(dataPath != "" && fIsNoiseOn){
        TFile* file = new TFile(dataPath, "READ");
        fNoiseTree = (TTree*)file -> Get("event");
        fNoiseTree -> SetBranchAddress("Noise", &fNoiseTemplate);
        fRandom = new TRandom3(0);

        fNoiseEventNum = fNoiseTree -> GetEntries();
        if(fNoiseEventNum == 0){
            fIsNoiseOn = false;
            cout << "STDSimTuningManager::InitNoiseShape() -- No Initialized Noise template.." << endl;
        }
        else{
            cout << "STDSimTuningManager::InitNoiseShape() -- Noise template has been initialized " << dataPath << endl;
        }
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

