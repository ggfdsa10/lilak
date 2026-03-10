#include "STDGarfieldInterface.h"

STDGarfieldInterface::STDGarfieldInterface()
{
    mSimFlag = kNon;

    // Default Gas mixture : He4 90% + CO2 10%
    mMixingGasName[0].first = "He4";
    mMixingGasName[0].second = 0.9;
    mMixingGasName[1].first = "CO2";
    mMixingGasName[1].second = 0.1;

    // Default gas systems
    mGasPressure = 760.;
    mGasTemperature = 293.15;
    mGasCollNum = 10;

    mFieldStepSize = 0.1; // [cm]

    mElmerMapPath = "../data/Elmer_TPCDrum_fieldcage_3000V";
    mOutFilePath = "../data/";
    mOutFileTag = "";
    mMultiNode = 1;
}

STDGarfieldInterface::~STDGarfieldInterface()
{
}

void STDGarfieldInterface::SetGas(TString mainGas, double MainGasFraction, TString subGas, double subGasFraction)
{
    mMixingGasName[0].first = mainGas;
    mMixingGasName[0].second = MainGasFraction;
    mMixingGasName[1].first = subGas;
    mMixingGasName[1].second = subGasFraction;
}

void STDGarfieldInterface::SetConvertFieldMap(double step)
{
    mFieldStepSize = step;
    mSimFlag = kConvertFieldMap;

    cout << "Field step size : " << step << " cm" << endl;
}

void STDGarfieldInterface::SetCalculateGasProperties(int eNum, double eMin, double eMax, int bNum, double bMin, double bMax)
{
    mEFieldBoundary[0] = eNum; 
    mEFieldBoundary[1] = eMin; 
    mEFieldBoundary[2] = eMax; 
    BFieldBoundary[0] = bNum; 
    BFieldBoundary[1] = bMin; 
    BFieldBoundary[2] = bMax; 
    mSimFlag = kGasProperties;

    cout << "E-Field (" << eMin << " - " << eMax << " V/cm) with #" <<  eNum << ", B-Field (" << bMin << " - " << bMax << " T) with #" << bNum << endl;
}

int STDGarfieldInterface::Init()
{
    InitGas();
    InitElmerFieldMap();
    InitOutputFile();
    
    return 1;
}

int STDGarfieldInterface::Calculate()
{
    if(mSimFlag == kConvertFieldMap){ConvertFieldMap();}
    if(mSimFlag == kGasProperties){CalculateGasProperties();}
        
    return 1;
}

int STDGarfieldInterface::Finish()
{
    mOutFile[0] -> Close();
    return 1;
}

void STDGarfieldInterface::InitGas()
{
    string mainGas = mMixingGasName[0].first.Data();
    double mainGasRatio = mMixingGasName[0].second;
    string subGas = mMixingGasName[1].first.Data();
    double subGasRatio = mMixingGasName[1].second;

    if(mainGasRatio + subGasRatio != 1.){
        subGasRatio = 1. - mainGasRatio;
    }

    mGasMedium = new MediumMagboltz(mainGas, mainGasRatio, subGas, subGasRatio);
    mGasMedium -> SetPressure(mGasPressure);
    mGasMedium -> SetTemperature(mGasTemperature);
    mGasMedium -> EnableAutoEnergyLimit(false);
	mGasMedium -> SetMaxElectronEnergy(1e21);

    cout << "STDGarfieldInterface::InitGas() -- MediumMagboltz Gas (" << mainGas << " " << mainGasRatio*100. << " % + " << subGas << " " << subGasRatio*100. << " %) has been initialized" << endl;
}

void STDGarfieldInterface::InitElmerFieldMap()
{
    mElmer = new ComponentElmer(Form("%s/mesh.header", mElmerMapPath.Data()),
                                Form("%s/mesh.elements", mElmerMapPath.Data()),
                                Form("%s/mesh.nodes", mElmerMapPath.Data()),
                                Form("%s/dielectrics.dat", mElmerMapPath.Data()),
                                Form("%s/TPC-Drum_fieldcage_3000V.result", mElmerMapPath.Data()), 
                                "mm");

    mElmer -> SetGas(mGasMedium);

    cout << "STDGarfieldInterface::InitElmerFieldMap() -- ComponentElmer has been initialized : " << mElmerMapPath << endl;
}

void STDGarfieldInterface::InitOutputFile()
{
    if(mOutFilePath[mOutFilePath.Sizeof()-1] != '/'){
        mOutFilePath = mOutFilePath + "/";
    }

    if(mSimFlag == kConvertFieldMap){
        mMultiNode = 1;
        
        TString outputFileName = mOutFilePath + "STDGarfield_FieldMap";
        if(mOutFileTag != ""){outputFileName += "_"+mOutFileTag;}
        outputFileName += ".root";

        mOutFile.push_back(new TFile(outputFileName, "recreate"));

        cout << "STDGarfieldInterface::InitOutputFile() -- Output file has been initialized : " << outputFileName << endl;
    }
    if(mSimFlag == kGasProperties){
        // vector<TFile*> mOutFile;
        // vector<TTree*> mOutTree;
    }
}

int STDGarfieldInterface::ConvertFieldMap()
{
    cout << " STDGarfieldInterface::ConvertFieldMap() -- Converting started " << endl;
    
    double minX, minY, minZ, maxX, maxY, maxZ; // elmer map boundaries
    mElmer -> GetBoundingBox(minX, minX, minZ, maxX, maxY, maxZ); // [cm]

    int binNumX = int((maxX - minX)/mFieldStepSize);
    int binNumY = int((maxY - minY)/mFieldStepSize);
    int binNumZ = int((maxZ - minZ)/mFieldStepSize);
    
    TH3D* fieldMap_ex = new TH3D("FieldMap_ex", "", binNumX, minX, maxX, binNumY, minY, maxY, binNumZ, minZ, maxZ);
    TH3D* fieldMap_ey = new TH3D("FieldMap_ey", "", binNumX, minX, maxX, binNumY, minY, maxY, binNumZ, minZ, maxZ);
    TH3D* fieldMap_ez = new TH3D("FieldMap_ez", "", binNumX, minX, maxX, binNumY, minY, maxY, binNumZ, minZ, maxZ);
    TH3D* fieldMap_v = new TH3D("FieldMap_v", "", binNumX, minX, maxX, binNumY, minY, maxY, binNumZ, minZ, maxZ);
    fieldMap_ex -> SetName("EFieldMap_ex");
    fieldMap_ey -> SetName("EFieldMap_ey");
    fieldMap_ez -> SetName("EFieldMap_ez");
    fieldMap_v -> SetName("EFieldMap_v");

    double x, y, z;
    double ex, ey, ez, v;
    Medium* m;
    int status;
    for(int ix=0; ix<binNumX; ix++){
        for(int iy=0; iy<binNumY; iy++){
            for(int iz=0; iz<binNumZ; iz++){
                x = minX + double(ix)*mFieldStepSize + (mFieldStepSize/2.);
                y = minY + double(iy)*mFieldStepSize + (mFieldStepSize/2.);
                z = minZ + double(iz)*mFieldStepSize + (mFieldStepSize/2.);
                mElmer -> ElectricField(x, y, z, ex, ey, ez, v, m, status);
                if(status < 0){continue;}
                
                fieldMap_ex -> Fill(x, y, z, ex);
                fieldMap_ey -> Fill(x, y, z, ey);
                fieldMap_ez -> Fill(x, y, z, ez);
                fieldMap_v -> Fill(x, y, z, v);
            }
        }
    }

    mOutFile[0] -> cd();
    fieldMap_ex -> Write();
    fieldMap_ey -> Write();
    fieldMap_ez -> Write();
    fieldMap_v -> Write();

    cout << "STDGarfieldInterface::ConvertFieldMap() -- E-Field map has been converted from Elmer to ROOT." << endl; 

    return 1;
}

int STDGarfieldInterface::CalculateGasProperties()
{
    
    return 1;
}