#include "TPCDrumConstruction.h"

TPCDrumConstruction::TPCDrumConstruction()
{
    ;
}

virtual G4VPhysicalVolume* TPCDrumConstruction::Construct()
{
    auto runManager = (LKG4RunManager *) G4RunManager::GetRunManager();
    auto par = runManager -> GetParameterContainer();

    G4double STPTemperature = 273.15;
    G4double labTemperature = STPTemperature + 20.*kelvin;

    G4Element *elementH = new G4Element("elementH", "H", 1., 1.00794*g/mole);
    G4Element *elementC = new G4Element("elementC", "C", 6., 12.011*g/mole);

    G4double densityArGas = 1.782e-3*g/cm3*STPTemperature/labTemperature;
    G4Material *matArGas = new G4Material("ArgonGas", 18, 39.948*g/mole, densityArGas, kStateGas, labTemperature);

    G4double densityMethane = 0.717e-3*g/cm3*STPTemperature/labTemperature;
    G4Material *matMethaneGas = new G4Material("matMethaneGas ", densityMethane, 2, kStateGas, labTemperature);
    matMethaneGas -> AddElement(elementH, 4);
    matMethaneGas -> AddElement(elementC, 1);

    G4double densityGas = .9*densityArGas + .1*densityMethane;
    matGas = new G4Material("matP10", densityGas, 2, kStateGas, labTemperature);
    matGas -> AddMaterial(matArGas, 0.9*densityArGas/densityGas);
    matGas -> AddMaterial(matMethaneGas, 0.1*densityMethane/densityGas);

    // nist manager is for predefined materials from
    // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html
    G4NistManager *nist = G4NistManager::Instance();
    G4Material *matAir = nist -> FindOrBuildMaterial("G4_AIR");

    G4Box *solidWorld = new G4Box("World", worlddX, worlddY, worlddZ);
    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, matAir, "World");
    G4PVPlacement *physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, -1, true);

    G4Tubs *solidDetector = new G4Tubs("ExTPC", tpcInnerRadius, tpcOuterRadius, .5*tpcLength, 0., 360*deg);
    G4LogicalVolume *logicDetector = new G4LogicalVolume(solidDetector, matGas, "ExTPC");
    logicDetector -> SetUserLimits(new G4UserLimits(1.*mm));
    auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,tpcZOffset), logicDetector, "ExTPC", logicWorld, false, 0, true);

    // Register to LKG4RunManager
    runManager -> SetSensitiveDetector(pvp);

    return physWorld;
}

