#include "TPCDrumConstruction.h"

TPCDrumConstruction::TPCDrumConstruction()
{
}

G4VPhysicalVolume* TPCDrumConstruction::Construct()
{
    auto runManager = (LKG4RunManager *) G4RunManager::GetRunManager();
    fPar = runManager -> GetParameterContainer();

    // =========================== world ================================ 
    double worldX = 1.* m;
    double worldY = 1.* m;
    double worldZ = 1.* m;
    G4Box* solidWorld = new G4Box("World", worldX, worldY, worldZ);
    G4Material* matAir = GetGasMaterial("AIR");
    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, matAir, "World");
    G4PVPlacement *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "World", 0, false, 0, true);

    // ========================== chamber ================================
    double FlangeR = 50.*mm; // flange radious
    double BeamLineLength = 620.*mm; // beamline+flange length along z-axis
    double chamberR = 504.*mm; // [mm] TPC-Drum chamber R
    double chamberHeight = 266.*mm; // [mm] TPC-Drum height

    G4Tubs* solidChamber = new G4Tubs("chamber", 0., chamberR/2., chamberHeight/2., 0., 360*deg);
    G4Tubs* solidFlange_Beamline = new G4Tubs("flange_beamline", 0., FlangeR/2., BeamLineLength/2., 0., 360*deg);
    G4UnionSolid* detector = new G4UnionSolid("TPCDrum", solidFlange_Beamline, solidChamber, new G4RotationMatrix(0., CLHEP::pi/2, 0.), G4ThreeVector(0., 0., 0.));

    G4Material* matGas = GetGasMaterial("HE4_CO2");

    G4LogicalVolume* logicChamber = new G4LogicalVolume(detector, matGas, "TPCDrum");
    logicChamber -> SetVisAttributes(GetColor("WHITE"));

    auto pvpChamber = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicChamber, "TPCDrum", logicWorld, false, 1, true);

    // ========================== Chamber window material =============================
    double windowThickness = fPar -> GetParDouble("TPCDrum/WindowThickness") * mm;
    double window_shiftZ = (BeamLineLength+windowThickness)/2.;

    G4Tubs* solidWindow = new G4Tubs("ChamberWindow", 0., FlangeR/2., windowThickness/2., 0., 360*deg);
    G4Material* MatWindow = GetSolidMaterial(fPar -> GetParString("TPCDrum/WindowMaterial"));
    G4LogicalVolume *logicWindow = new G4LogicalVolume(solidWindow, MatWindow, "ChamberWindow");
    logicWindow -> SetVisAttributes(GetColor("WHITE", 0.6));
    auto pvpWindow_UpStream = new G4PVPlacement(0, G4ThreeVector(0., 0., -window_shiftZ), logicWindow, "ChamberWindow_upstream", logicWorld, false, 2, true);
    auto pvpWindow_DownStream = new G4PVPlacement(0, G4ThreeVector(0.,0., window_shiftZ), logicWindow, "ChamberWindow_downstream", logicWorld, false, 3, true);

    // ========================== readout PadPlane PCB and Pad ================================
    const double readoutPCBThickness = 4.2*mm;
    const double readoutPCBR = 370.*mm;
    const double readOutPCB_shiftZ = chamberHeight/2.-4.2/2;
    const double readOutPadAreaY = 144. *mm;
    const double readOutPadAreaX = 74. *mm;

    G4Tubs* solidReadoutPCB = new G4Tubs("ReadOutPCB", 0., readoutPCBR/2., readoutPCBThickness/2., 0., 360*deg);
    G4Material* matPCB = GetSolidMaterial("ReadOutPCB");
    G4LogicalVolume *logicReadoutPCB = new G4LogicalVolume(solidReadoutPCB, matPCB, "ReadOutPCB");
    logicReadoutPCB -> SetVisAttributes(GetColor("BLUE"));
    auto pvpReadoutPCB = new G4PVPlacement(new G4RotationMatrix(0., CLHEP::pi/2, 0.), G4ThreeVector(0., -readOutPCB_shiftZ, 0.), logicReadoutPCB, "ReadOutPCB", logicChamber, false, 4, true);

    G4Box* solidReadoutPad = new G4Box("ReadOutPad", readOutPadAreaX/2., readoutPCBThickness/2., readOutPadAreaY/2.);
    G4LogicalVolume *logicReadoutPad = new G4LogicalVolume(solidReadoutPad, matPCB, "ReadOutPad");
    logicReadoutPad -> SetVisAttributes(GetColor("YELLOW", 0.4));
    auto pvpReadoutPad = new G4PVPlacement(0, G4ThreeVector(0., -readOutPCB_shiftZ, 0.), logicReadoutPad, "ReadOutPad", logicChamber, false, 5, true);

    // Tripple GEM area
    const double gemSpaceLength = 200.*mm;
    const double gemSpaceHeight= 3.*mm;

    // ========================== Field Cage ================================
    const double fieldCageLength = 200.*mm; // field cage length
    const double fieldCageHeight = 150.*mm; // field cage height
    const double fieldCageSpacing = 5.*mm; // field cage spacing 
    const double fieldCage_shiftZ = (chamberHeight/2. - fieldCageHeight/2. - readoutPCBThickness - gemSpaceHeight*3. - fieldCageSpacing)*mm;
    const double fieldCagePCB_Thickness = 2*mm;

    G4VisAttributes* greenColor = GetColor("GREEN", 0.1);
    greenColor -> SetLineWidth(2);

    // FieldCage left PCB 
    G4Box* solidFieldCage_LeftPCB = new G4Box("fieldCage_leftPCB", fieldCagePCB_Thickness/2., fieldCageHeight/2., fieldCageLength/2.);
    G4LogicalVolume* logicFieldCage_LeftPCB = new G4LogicalVolume(solidFieldCage_LeftPCB, matPCB, "fieldCage_leftPCB");
    logicFieldCage_LeftPCB -> SetVisAttributes(greenColor);
    auto pvpFieldCage_LeftPCB = new G4PVPlacement(new G4RotationMatrix(0., 0., 0.), G4ThreeVector((1+fieldCageLength/2.)*mm, -fieldCage_shiftZ, 0.), logicFieldCage_LeftPCB, "fieldCage_leftPCB", logicChamber, false, 6, true);

    // FieldCage Right PCB 
    G4Box* solidFieldCage_RightPCB = new G4Box("fieldCage_RightPCB", fieldCagePCB_Thickness/2., fieldCageHeight/2., fieldCageLength/2.);
    G4LogicalVolume* logicFieldCage_RightPCB = new G4LogicalVolume(solidFieldCage_RightPCB, matPCB, "fieldCage_RightPCB");
    logicFieldCage_RightPCB -> SetVisAttributes(greenColor);
    auto pvpFieldCage_RightPCB = new G4PVPlacement(new G4RotationMatrix(0., 0., 0.), G4ThreeVector(-(1+fieldCageLength/2.)*mm, -fieldCage_shiftZ, 0.), logicFieldCage_RightPCB, "fieldCage_RightPCB", logicChamber, false, 7, true);

    // FieldCage Cathode PCB 
    G4Box* solidFieldCage_CathodePCB = new G4Box("fieldCage_CathodePCB", fieldCageLength/2., fieldCagePCB_Thickness/2., fieldCageLength/2.);
    G4LogicalVolume* logicFieldCage_CathodePCB = new G4LogicalVolume(solidFieldCage_CathodePCB, matPCB, "fieldCage_CathodePCB");
    logicFieldCage_CathodePCB -> SetVisAttributes(greenColor);
    auto pvpFieldCage_CathodePCB = new G4PVPlacement(new G4RotationMatrix(0., 0., 0.), G4ThreeVector(0., -fieldCage_shiftZ+fieldCageHeight/2., 0.), logicFieldCage_CathodePCB, "fieldCage_CathodePCB", logicChamber, false, 8, true);

    // FieldCage Downstream PCB
    const double fieldCage_downstream_emptySizeX = 170.*mm;
    const double fieldCage_downstream_emptySizeY = 130.*mm;
    G4Box* solidFieldCage_DownStreamPCB_base = new G4Box("fieldCage_downstreamPCB_base", fieldCageLength/2., fieldCageHeight/2., fieldCagePCB_Thickness/2.);
    G4Box* solidFieldCage_DownStreamPCB_subt = new G4Box("fieldCage_downstreamPCB_subt", fieldCage_downstream_emptySizeX/2., fieldCage_downstream_emptySizeY/2., fieldCagePCB_Thickness/2+1.*mm);
    G4VSolid* SolidFieldCage_DownStreamPCB = new G4SubtractionSolid("fieldCage_downstreamPCB", solidFieldCage_DownStreamPCB_base, solidFieldCage_DownStreamPCB_subt, 0,  G4ThreeVector(0,0,0));

    G4LogicalVolume* logicFieldCage_DownStreamPCB = new G4LogicalVolume(SolidFieldCage_DownStreamPCB, matPCB, "fieldCage_downstreamPCB");
    logicFieldCage_DownStreamPCB -> SetVisAttributes(greenColor);
    auto pvpFieldCage_DownStreamtPCB = new G4PVPlacement(new G4RotationMatrix(0., 0., 0.), G4ThreeVector(0., -fieldCage_shiftZ, (1+fieldCageLength/2.)*mm), logicFieldCage_DownStreamPCB, "fieldCage_downstreamPCB", logicChamber, false, 9, true);
    
    // FieldCage Upstream PCB
    const double fieldCage_downstream_WindowSize = 50.*mm;
    const double fieldCage_downstream_WindowHeight = 107.*mm;
    G4Box* solidFieldCage_UpStreamPCB_base = new G4Box("fieldCage_UpstreamPCB_base", fieldCageLength/2., fieldCageHeight/2., fieldCagePCB_Thickness/2.);
    G4Box* solidFieldCage_UpStreamPCB_subt = new G4Box("fieldCage_UpstreamPCB_subt", fieldCage_downstream_WindowSize/2., fieldCage_downstream_WindowSize/2., fieldCagePCB_Thickness/2.+1.*mm);
    G4VSolid* SolidFieldCage_UpStreamPCB = new G4SubtractionSolid("fieldCage_UpstreamPCB", solidFieldCage_UpStreamPCB_base, solidFieldCage_UpStreamPCB_subt, 0,  G4ThreeVector(0,32.*mm,0));

    G4LogicalVolume* logicFieldCage_UpStreamPCB = new G4LogicalVolume(SolidFieldCage_UpStreamPCB, matPCB, "fieldCage_UpstreamPCB");
    logicFieldCage_UpStreamPCB -> SetVisAttributes(greenColor);
    auto pvpFieldCage_UpStreamtPCB = new G4PVPlacement(new G4RotationMatrix(0., 0., 0.), G4ThreeVector(0., -fieldCage_shiftZ, -(1+fieldCageLength/2.)*mm), logicFieldCage_UpStreamPCB, "fieldCage_UpstreamPCB", logicChamber, false, 10, true);


    // ========================== Gating Grid ======================================
    if(fPar->CheckPar("TPCDrum/GatingGrid")){
        if(fPar->GetParBool("TPCDrum/GatingGrid")){
            const double GatingGrid_Supporter_Width = 20.*mm;
            const double GatingGrid_Width = 20.*mm;
            const double GatingGrid_AcitveArea_Width = fieldCageLength/2.-GatingGrid_Supporter_Width-GatingGrid_Width/2.-fieldCagePCB_Thickness;
            const double GatingGrid_emptyArea_Center = GatingGrid_AcitveArea_Width/2.+GatingGrid_Width/2.+fieldCagePCB_Thickness;
            G4Box* solidGatingGrid_base = new G4Box("gatingGrid_base", fieldCageLength/2., fieldCagePCB_Thickness/2., fieldCageLength/2.);
            G4Box* solidGatingGrid_subtEmptyActive = new G4Box("gatingGrid_subtEmptyActive", GatingGrid_AcitveArea_Width/2., fieldCagePCB_Thickness/2.+1.*mm, (fieldCageLength-2*GatingGrid_Supporter_Width)/2.);
            G4Box* solidGatingGrid_subtEmptyCenter = new G4Box("GatingGrid_subtEmptyCenter", GatingGrid_Width/2., fieldCagePCB_Thickness/2.+1.*mm, (fieldCageLength-2*GatingGrid_Supporter_Width)/2.);
            G4VSolid* SolidGatingGrid_subtRight = new G4SubtractionSolid("GatingGrid_tmp", solidGatingGrid_base, solidGatingGrid_subtEmptyActive, 0,  G4ThreeVector(GatingGrid_emptyArea_Center,0,0));
            G4VSolid* SolidGatingGrid_subtLeft = new G4SubtractionSolid("GatingGrid_tmp", SolidGatingGrid_subtRight, solidGatingGrid_subtEmptyActive, 0,  G4ThreeVector(-GatingGrid_emptyArea_Center,0,0));
            G4VSolid* SolidGatingGrid = new G4SubtractionSolid("GatingGrid_tmp", SolidGatingGrid_subtLeft, solidGatingGrid_subtEmptyCenter, 0,  G4ThreeVector(0,0,0));

            G4LogicalVolume* logicGatingGrid = new G4LogicalVolume(SolidGatingGrid, matPCB, "GatingGrid");
            logicGatingGrid -> SetVisAttributes(GetColor("GREEN",0.4));
            auto pvpFieldCage_UpStreamtPCB = new G4PVPlacement(new G4RotationMatrix(0., 0., 0.), G4ThreeVector(0., -fieldCage_shiftZ, 0.), logicGatingGrid, "GatingGrid", logicChamber, false, 11, true);
        }
    }

    // ========================== Silicon telespoce ================================

    // ====================== NPTool =======================
    G4ProductionCuts* ecut = new G4ProductionCuts();
    double eEnergyThreshold = 40.*MeV;
    ecut->SetProductionCut(eEnergyThreshold,"e-");
    fReactionRegion = new G4Region("NPSimulationProcess");
    fReactionRegion->SetProductionCuts(ecut);
    fReactionRegion->AddRootLogicalVolume(logicChamber);
    fReactionRegion->SetUserLimits(new G4UserLimits(0.1 * mm));    

    G4FastSimulationManager* mng = fReactionRegion->GetFastSimulationManager();
    unsigned int size = fReactionModel.size();
    for (unsigned int i = 0; i < size; i++) {
        mng->RemoveFastSimulationModel(fReactionModel[i]);
    }
    fReactionModel.clear();

    G4VFastSimulationModel* fsm;
    fsm = new NPS::BeamReaction("BeamReaction", fReactionRegion);
    fReactionModel.push_back(fsm);
    fsm = new NPS::Decay("Decay", fReactionRegion);
    fReactionModel.push_back(fsm);

    runManager -> SetSensitiveDetector(pvpChamber);
    return physWorld;
}

G4LogicalVolume* TPCDrumConstruction::GetSiDetector(TString name)
{   
    const double siX = 40.3 * mm;
    const double SiY = 75 * mm;
    const double SiThickness = 1. * mm;

    G4Box *solidSi = new G4Box(name.Data(), siX, SiY, SiThickness);
    G4Material* metSi = GetSolidMaterial("Si");
    G4LogicalVolume* logicSi = new G4LogicalVolume(solidSi, metSi, name.Data());

    return logicSi;
}

G4Material* TPCDrumConstruction::GetGasMaterial(TString gasName)
{
    gasName.ToUpper();

    G4double torrToAtom = 1./760. * CLHEP::STP_Pressure;
    G4double pressure = fPar -> GetParDouble("TPCDrum/GasPressure") * torrToAtom; // [atom]
    G4double mixRatio = fPar -> GetParDouble("TPCDrum/GasContourMixRatio") * 0.01;

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

G4Material* TPCDrumConstruction::GetSolidMaterial(TString name)
{
    G4NistManager *nist = G4NistManager::Instance();

    name.ToUpper();
    if(name.Index("PCB") != -1){
        double FR4Density        = 1.85 * g/cm3;
        double EpoxyDensity      = 1.25 * g/cm3;
        double fiberGlassDensity = 1.99 * g/cm3;

        double SiO2Density  = 2.196 * g/cm3;
        double B2O3Density  = 1.85  * g/cm3;
        double Al2O3Density = 3.987 * g/cm3;
        double CaODensity   = 3.34  * g/cm3;

        G4Element* H = nist -> FindOrBuildElement("H", false);
        G4Element* B = nist -> FindOrBuildElement("B", false);
        G4Element* C = nist -> FindOrBuildElement("C", false);
        G4Element* O = nist -> FindOrBuildElement("O", false);
        G4Element* Al = nist -> FindOrBuildElement("Al", false);
        G4Element* Si = nist -> FindOrBuildElement("Si", false);
        G4Element* Ca = nist -> FindOrBuildElement("Ca", false);

        // for Epoxy
        G4Material* epoxy = new G4Material("epoxy", EpoxyDensity, 2);
        epoxy -> AddElement(H, 2);
        epoxy -> AddElement(C, 2);

        // // for FiberGlass
        G4Material* SiO2 = new G4Material("SiO2", SiO2Density, 2);
        SiO2 -> AddElement(Si, 1);
        SiO2 -> AddElement(O, 2);

        G4Material* B2O3 = new G4Material("B2O3", B2O3Density, 2);
        B2O3 -> AddElement(B, 2);
        B2O3 -> AddElement(O, 3);

        G4Material* Al2O3 = new G4Material("Al2O3", Al2O3Density, 2);
        Al2O3 -> AddElement(Al, 2);
        Al2O3 -> AddElement(O, 3);

        G4Material* CaO = new G4Material("CaO", CaODensity, 2);
        CaO -> AddElement(Ca, 1);
        CaO -> AddElement(O, 1);

        G4Material* FiberGlass = new G4Material("FIberGlass", fiberGlassDensity, 4);
        FiberGlass -> AddMaterial(SiO2, 0.6*SiO2Density/fiberGlassDensity);
        FiberGlass -> AddMaterial(B2O3, 0.05*B2O3Density/fiberGlassDensity);
        FiberGlass -> AddMaterial(Al2O3, 0.13*Al2O3Density/fiberGlassDensity);
        FiberGlass -> AddMaterial(CaO, 0.22*CaODensity/fiberGlassDensity);

        G4Material* PCB_FR4 = new G4Material("PCB_FR4", FR4Density, 2);
        PCB_FR4 -> AddMaterial(epoxy, 0.4*EpoxyDensity/FR4Density);
        PCB_FR4 -> AddMaterial(FiberGlass, 0.6*fiberGlassDensity/FR4Density);

        return PCB_FR4;
    }
    else if(name.Index("Si") != -1){
        double SiDetectorDensity = 2.33 *g/cm3;

        G4Element* Si = nist -> FindOrBuildElement("Si", false);

        G4Material* SiDetector = new G4Material("SiDetector", SiDetectorDensity, 1);
        SiDetector -> AddElement(Si, 1);

        return SiDetector;
    }
    else if(name.Index("Haver") != -1){
        double HavarFoilDensity = 8.3 *g/cm3;

        G4Element* Co = nist -> FindOrBuildElement("Co", false);
        G4Element* Cr = nist -> FindOrBuildElement("Cr", false);
        G4Element* Ni = nist -> FindOrBuildElement("Ni", false);
        G4Element* Fe = nist -> FindOrBuildElement("Fe", false);
        G4Element* W = nist -> FindOrBuildElement("W", false);
        G4Element* Mo = nist -> FindOrBuildElement("Mo", false);
        G4Element* Mn = nist -> FindOrBuildElement("Mn", false);
        G4Element* C = nist -> FindOrBuildElement("C", false);
        G4Element* Be = nist -> FindOrBuildElement("Be", false);

        G4Material* Havar = new G4Material("Havar", HavarFoilDensity, 9);
        Havar -> AddElement(Co, 0.42);
        Havar -> AddElement(Cr, 0.195);
        Havar -> AddElement(Ni, 0.127);
        Havar -> AddElement(Fe, 0.1905);
        Havar -> AddElement(W, 0.027);
        Havar -> AddElement(Mo, 0.022);
        Havar -> AddElement(Mn, 0.016);
        Havar -> AddElement(C, 0.002);
        Havar -> AddElement(Be, 0.0005);

        return Havar;
    }
}

G4VisAttributes* TPCDrumConstruction::GetColor(TString color, double transparency)
{
    G4VisAttributes* visAtt = new G4VisAttributes();
    color.ToUpper();
    if(color.Index("WHITE") != -1){
        visAtt -> SetColor(1., 1., 1., transparency);
        return visAtt;
    }
    else if(color.Index("GRAY") != -1){
        visAtt -> SetColor(0.5, 0.5, 0.5, transparency);
        return visAtt;
    }
    else if(color.Index("RED") != -1){
        visAtt -> SetColor(1., 0., 0., transparency);
        return visAtt;
    }
    else if(color.Index("BLUE") != -1){
        visAtt -> SetColor(0., 0., 1., transparency);
        return visAtt;
    }
    else if(color.Index("GREEN") != -1){
        visAtt -> SetColor(0., 1., 0., transparency);
        return visAtt;
    }
    else if(color.Index("YELLOW") != -1){
        visAtt -> SetColor(1., 1., 0., transparency);
        return visAtt;
    }
    else if(color.Index("MAGENTA") != -1){
        visAtt -> SetColor(1., 0., 1., transparency);
        return visAtt;
    }
    else if(color.Index("CYAN") != -1){
        visAtt -> SetColor(0., 1., 1., transparency);
        return visAtt;
    }
    visAtt -> SetColor(0., 0., 0., transparency);
    return visAtt;
}