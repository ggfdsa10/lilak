#ifndef STDGarfieldInterface_hh
#define STDGarfieldInterface_hh

#include <iostream>
#include <vector>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TH3D.h"

#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ComponentElmer.hh"
#include "Garfield/Sensor.hh"

using namespace std;
using namespace Garfield;

class STDGarfieldInterface
{
    enum SimFlag
    {
        kNon = -1,
        kConvertFieldMap = 0,
        kGasProperties = 1
    };

    public:
        STDGarfieldInterface();
        ~STDGarfieldInterface();

        // Note: It is a important values to match Elmer to local pad coordinate 
        // Please ensure the local pad coordinate from Elmer coordinate
        void SetLocalPadCoordinateShift(double xshift, double yshift, double zshift); 

        void SetGas(TString mainGas, double MainGasFraction, TString subGas="", double subGasFraction=0.); // default He4 90% + CO2 10%
        void SetGasPressure(double torr){mGasPressure = torr;}           // default 760 Torr
        void SetGasTemperature(double kelvin){mGasTemperature = kelvin;} // default 293.15 K
        void SetCollisionNum(int order){mGasCollNum = order;}            // default 10^{order = 10} 

        void SetElmerMapPath(TString path){mElmerMapPath = path;} // full path
        void SetOutputNameTag(TString tag){mOutFileTag = tag;}

        void SetMultiNode(int n){mMultiNode = n;} // multi-precessing node num

        void SetConvertFieldMap(double step=0.1); // step size [cm]
        void SetCalculateGasProperties(int eNum, double eMin, double eMax, int bNum=0, double bMin=0., double bMax=0.); // [V/cm, T]

        int Init();
        int Calculate();
        int Finish();

    private:
        void InitGas();
        void InitElmerFieldMap();
        void InitOutputFile();

        int ConvertFieldMap();
        int CalculateGasProperties();

        vector<TFile*> mOutFile;
        vector<TTree*> mOutTree;

        int mSimFlag;

        MediumMagboltz* mGasMedium;
        ComponentElmer* mElmer;

        pair<TString, double> mMixingGasName[2]; // [main, sub]
        double mGasPressure;
        double mGasTemperature;
        int mGasCollNum;

        double mFieldStepSize; // [cm]
        double mCoordinateShift[3]; // [x, y, z]
        double mEFieldBoundary[3]; // [num, min, max]
        double BFieldBoundary[3]; // [num, min, max]

        TString mElmerMapPath;
        TString mOutFilePath;
        TString mOutFileTag;
        int mMultiNode;

        const double cm2mm = 10.; // cm to mm conversion factor
        //const double cmus2mmns = ;
        //const double cmPerSqrtcm2 = ;


};

#endif