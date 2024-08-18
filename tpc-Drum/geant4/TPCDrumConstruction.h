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

/*
 * Remove this comment block after reading it through
 * or use print_example_comments=False option to omit printing
 *
 * # Example Geatn4 detector construction class
 *
 * This is normal geant4 detector construction class.
 * One additional thing from creating normal geant4 class is that
 * you should add your detector to the run manager using LKG4RunManager::SetSensitiveDetector(G4PVPlacement) method
 *
 * - Write Construct() method.
 * - In Construct(), you should add detector to LKG4RunManager using LKG4RunManager::SetSensitiveDetector(G4PVPlacement) method.
 */

class TPCDrumConstruction : public G4VUserDetectorConstruction
{
    public:
        TPCDrumConstruction();
        virtual ~TPCDrumConstruction() { ; }

        virtual G4VPhysicalVolume* Construct();

};

#endif
