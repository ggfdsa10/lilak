#include "TPCDrumConstruction.h"

TPCDrumConstruction::TPCDrumConstruction()
{
}

G4VPhysicalVolume* TPCDrumConstruction::Construct()
{
    auto runManager = (LKG4RunManager *) G4RunManager::GetRunManager();
    mPar = runManager -> GetParameterContainer();

    // =========================== world ================================ 
    double worldX = 1.* m;
    double worldY = 1.* m;
    double worldZ = 1.* m;
    G4Box *solidWorld = new G4Box("World", worldX, worldY, worldZ);
    G4Material* matAir = GetGasMaterial("AIR");
    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, matAir, "World");
    G4PVPlacement *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "World", 0, false, 0, true);

    // ========================== chamber ================================
    double chamberR = 504.*mm; // [mm] TPC-Drum chamber R
    double chamberHeight = 266.*mm; // [mm] TPC-Drum height
    double windowFlangeR = 50.*mm;
    double windowFLangeL = 60.*mm;

    G4Tubs* solidChamber = new G4Tubs("chamber", 0., chamberR/2., chamberHeight/2., 0., 360*deg);
    G4Tubs* solidFlange_upStream = new G4Tubs("flange", 0., windowFlangeR/2., windowFLangeL/2., 0., 360*deg);

    // G4UnionSolid* detector = new G4UnionSolid("TPCDrum", solidChamber, solidFlange_upStream, new G4RotationMatrix(0., CLHEP::pi/2, 0.), G4ThreeVector(0.,chamberR/2.,0.));

    G4Material* matGas = GetGasMaterial("HE4_CO2");
    G4LogicalVolume *logicChamber = new G4LogicalVolume(solidChamber, matGas, "TPCDrum");
    auto pvp = new G4PVPlacement(new G4RotationMatrix(0., -1.*CLHEP::pi/2, 0.), G4ThreeVector(0.,0.,0.), logicChamber, "TPCDrum", logicWorld, false, 1, true);
    // auto pvp = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicChamber, "TPCDrum", logicWorld, false, 1, true);


    // ====================== NPTool =======================
    G4ProductionCuts* ecut = new G4ProductionCuts();
    double eEnergyThreshold = 1000.*MeV;
    ecut->SetProductionCut(eEnergyThreshold,"e-");
    mReactionRegion = new G4Region("NPSimulationProcess");
    mReactionRegion->SetProductionCuts(ecut);
    mReactionRegion->AddRootLogicalVolume(logicChamber);
    mReactionRegion->SetUserLimits(new G4UserLimits(1. * mm));    

    G4FastSimulationManager* mng = mReactionRegion->GetFastSimulationManager();
    unsigned int size = mReactionModel.size();
    for (unsigned int i = 0; i < size; i++) {
        mng->RemoveFastSimulationModel(mReactionModel[i]);
    }
    mReactionModel.clear();

    G4VFastSimulationModel* fsm;
    fsm = new NPS::BeamReaction("BeamReaction", mReactionRegion);
    mReactionModel.push_back(fsm);
    fsm = new NPS::Decay("Decay", mReactionRegion);
    mReactionModel.push_back(fsm);

    runManager -> SetSensitiveDetector(pvp);
    return physWorld;
}

G4Material* TPCDrumConstruction::GetGasMaterial(TString gasName)
{
    gasName.ToUpper();

    G4double torrToAtom = 1./760. * CLHEP::STP_Pressure;
    G4double pressure = mPar -> GetParDouble("TPCDrum/GasPressure") * torrToAtom; // [atom]
    G4double mixRatio = mPar -> GetParDouble("TPCDrum/GasContourMixRatio") * 0.01;

    G4double STPTemperature = 273.15;
    G4double labTemperature = STPTemperature + 20.*kelvin;

    G4double gasDensity_He4 = 0.0001785 * STPTemperature/labTemperature *g/cm3;
    G4double gasDensity_CO2 = 0.001977 * STPTemperature/labTemperature *g/cm3;

    G4NistManager *nist = G4NistManager::Instance();
    G4Element* helium = nist -> FindOrBuildElement("He", false);
    G4Element* carbon = nist -> FindOrBuildElement("C", false);
    G4Element* oxygen = nist -> FindOrBuildElement("O", false);

    G4Material* air = nist -> FindOrBuildMaterial("G4_AIR");
    G4Material* He4Gas = new G4Material("He4Gas", gasDensity_He4, 1, kStateGas, labTemperature, pressure);
    He4Gas -> AddElement(helium, 1);
    G4Material* CO2Gas = new G4Material("CO2Gas", gasDensity_CO2, 2, kStateGas, labTemperature, pressure);
    CO2Gas -> AddElement(carbon, 1);
    CO2Gas -> AddElement(oxygen, 2);

    if(gasName.Index("AIR") != -1){return air;}
    else if(gasName.Index("HE4_CO2") != -1){
        G4double gasDensity_He4_CO2 = (1. - mixRatio) * gasDensity_He4 + mixRatio * gasDensity_CO2;
        G4Material* he4_co2_gas = new G4Material("He4_Co2_gas", gasDensity_He4_CO2, 2, kStateGas, labTemperature, CLHEP::STP_Pressure);
        he4_co2_gas -> AddMaterial(He4Gas, (1. - mixRatio)*gasDensity_He4/gasDensity_He4_CO2);
        he4_co2_gas -> AddMaterial(CO2Gas, mixRatio*gasDensity_CO2/gasDensity_He4_CO2);

        return  he4_co2_gas;
    }
    
    G4Material* vacuum = new G4Material("vacuum", 1, 2.016*g/mole, 1.e-10, kStateGas, labTemperature, 1.e-10*CLHEP::STP_Pressure);
    return vacuum;
}