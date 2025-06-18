#ifndef TPCDRUMCONSTRUCTION_HH
#define TPCDRUMCONSTRUCTION_HH

#include "LKG4RunManager.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"

#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4Tubs.hh"
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VisAttributes.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4FastSimulationManager.hh"
#include "G4VFastSimulationModel.hh"
#include "G4ProductionCuts.hh"
#include "globals.hh"

#include "STDPadPlane.h"

#include "DetectorConstruction.hh"
#include "BeamReaction.hh"
#include "Decay.hh"


class TPCDrumConstruction : public DetectorConstruction
{
    public:
        TPCDrumConstruction();
        virtual ~TPCDrumConstruction(){}

        G4VPhysicalVolume* Construct();

        G4LogicalVolume* GetGatingGrid();

    private:
        G4LogicalVolume* GetWorld();
        G4LogicalVolume* GetTPCDrumChamber();
        G4LogicalVolume* GetBeamWindow();
        G4LogicalVolume* GetReadoutPCB();
        G4LogicalVolume* GetReadoutPadPlane();
        G4LogicalVolume* GetFieldCageSideWall();
        G4LogicalVolume* GetFieldCageCathode();
        G4LogicalVolume* GetFieldCageUpStream();
        G4LogicalVolume* GetFieldCageDownStream();


        G4LogicalVolume* GetSiDetector(TString name);

        G4Material* GetGasMaterial(TString gasName);
        G4Material* GetSolidMaterial(TString name);

        G4VisAttributes* GetColor(TString color, double transparency=0.2);
        
        LKParameterContainer* fPar;
        G4Region* fReactionRegion; 
        vector<G4VFastSimulationModel*> fReactionModel;

    private: 
        // Geometry constant parameters
        // Note: parameter size presents the dimater and length
        const double fHalfUnit = 0.5;

        // World size 
        const double fWorldX = 2. *m;
        const double fWorldY = 2. *m;
        const double fWorldZ = 2. *m;

        // Chamber size
        const double fBeamLineLength = 620. *mm; // beamline+flange length along z-axis
        const double fChamberHeight = 266. *mm; // TPC-Drum chamber height
        const double fChamberR = 504. *mm; // TPC-Drum chamber diameter
        const double fFlangeR = 50. *mm; // chamber beam flange diameter

        // Readout PCB board
        const double fPCBThickness = 2. *mm;
        const double fReadoutPCBThickness = 4.2 *mm;
        const double fReadoutPCBR = 370. *mm;

        // Triple GEM
        const double fGEMSpacing = 3. *mm;

        // FieldCage 
        const double fFieldCageLength = 200. *mm; // field cage length
        const double fFieldCageHeight = 150. *mm; // field cage height
        const double fFieldCageSpacing = 5. *mm; // field cage spacing from triple GEM surface

        // GatingGrid
        const double fGatingGridSupporterWidth = 20. *mm;
        const double fGatingGridActiveWidth = 20. *mm;
        const double fGatingGridActivePCBWidth = 2. *mm;
    };

#endif