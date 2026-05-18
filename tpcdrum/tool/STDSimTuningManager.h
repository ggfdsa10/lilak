#ifndef STDSimTuningManager_HH
#define STDSimTuningManager_HH

#include "LKRun.h"
#include "LKParameterContainer.h"

#include "STDFieldDistortionMaker.h"
#include "STDGainCalibrationMaker.h"

#include "TF1.h"
#include "TMath.h"

class STDSimTuningManager
{
    public:
        static STDSimTuningManager* GetSimTuningManager();

        STDSimTuningManager();
        ~STDSimTuningManager(){}

        bool Init();

        void SetFieldDostortionMaker(STDFieldDistortionMaker* maker){fFieldDistortion = maker;}
        void SetGainCalibrationMaker(STDGainCalibrationMaker* maker){fGainCalibration = maker;}
        double GetWValue(); // [eV]

        double GetElectronStepSize(){return fElectronStepSize;}

        // local pad plane coordinate
        double GetDriftVelocity(double x, double y, double z); // V_{D} [mm/us]
        double GetDiffusionT(double x, double y, double z);    // D_{T} [mm/sqrt{mm}]
        double GetDiffusionL(double x, double y, double z);    // D_{L} [mm/sqrt{mm}]

        double GetGEMGainFactor(double x, double y, double z);
        double GetGEMDiffusionT();
        double GetGEMDiffusionL();
        double GetGEMDriftVelocity();

        double GetExtraDiffusionT();

    private:
        void InitGarfieldGasData();
        void InitGEMGain();
        void InitPulseShape();
        void InitNoiseShape();

        double GetGEMGain();
        Double_t PolyaDistribution(Double_t* x, Double_t* par); // Gain fluctuation (Polya distribution)

        static STDSimTuningManager* fInstance;
        LKRun* fRun;
        LKParameterContainer* fPar;

        double fElectronStepSize; // [mm]
        
        STDFieldDistortionMaker* fFieldDistortion;
        STDGainCalibrationMaker* fGainCalibration;

        bool fIsInitGarfieldData;
        TGraph* mVelocityData;
        TGraph* mTransDiffusionData;
        TGraph* mLongiDiffusionData;

        TF1* fGEMGainDist;
        
};

#endif