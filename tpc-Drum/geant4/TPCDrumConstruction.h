#ifndef TPCDRUMCONSTRUCTION_HH
#define TPCDRUMCONSTRUCTION_HH

#include "LKG4RunManager.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"

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
#include "globals.hh"

#include "DetectorConstruction.hh"

class TPCDrumConstruction : public DetectorConstruction
{
    public:
        TPCDrumConstruction();
        virtual ~TPCDrumConstruction() { ; }

        G4VPhysicalVolume* Construct();

};

#endif

