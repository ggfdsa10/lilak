#ifndef TPCDRUM_HH
#define TPCDRUM_HH

#include "LKDetector.h"
#include "LKLogger.h"

#include "STDPadPlane.h"
#include "STDSiArray.h"

class TPCDrum : public LKDetector
{
    public:
        TPCDrum();
        virtual ~TPCDrum() { ; }

        void Print(Option_t *option="") const;
        bool Init();
        bool BuildGeometry();
        bool BuildDetectorPlane();
        bool IsInBoundary(Double_t x, Double_t y, Double_t z); // Local coordinate

        bool IsDeadChan(int conifg, int padID);

        void GetCoordinateGeantToPad(double& x, double& y, double& z); // [mm] convertion coordinate system from geant global to local (pad plane)
        void GetCoordinatePadToGeant(double& x, double& y, double& z); // [mm] convertion coordinate system from local (pad plane) to geant global

        double GetFieldCageGeantYPos(); // [mm] field acge height along drift length (geant coordinate)
        double GetGatingGridGeantYPos(); // [mm] gating grid height along drift length (geant coordinate)

    private:
        bool IsDeadChanForMainRun_OldZAP(int padID); // test !!!

        STDPadPlane* fPadPlane;

    public:
        // ================== TPC-Drum Geometry parameters [mm] =====================
        // Chamber size
        const double fBeamLineLength = 620.; // beamline+flange length along beam-axis
        const double fChamberHeight = 266.; // TPC-Drum chamber height
        const double fChamberR = 504.; // TPC-Drum chamber diameter
        const double fFlangeR = 50.; // chamber beam flange diameter

        // Readout PCB board
        const double fPCBThickness = 2.; // general PCB thinckness
        const double fReadoutPCBThickness = 4.2; // readout PCB thickness 
        const double fReadoutPCBR = 370.; // readout PCB diameter

        // Triple GEM
        const double fGEMSpacing = 3.; // distance between GEMs

        // FieldCage 
        const double fFieldCageLength = 200.; // field cage length
        const double fFieldCageHeight = 150.; // field cage height
        const double fFieldCageSpacing = 4.; // distance between triple GEM surface and fieldcage
        const double fFieldCageDownstreamWindowSizeX = 170.; // field cage downstream window size x-axis
        const double fFieldCageDownstreamWindowSizeY = 130.; // field cage downstream window size drift length axis
        const double fFieldCageUpstreamWindowSize = 50.; // field cage upstream window size
        const double fFieldCageUpstreamWindowHeight = 107.; // field cage upstream window height with respect to fieldcage center

        // GatingGrid
        const double fGatingGridSupporterWidth = 20.; // gating grid boundary PCB supporter width
        const double fGatingGridActiveWidth = 18.; // gating grid wire area width 
        const double fGatingGridActivePCBWidth = 3.; // gating grid wire supporter PCB width
        
    ClassDef(TPCDrum,1);
};

#endif
