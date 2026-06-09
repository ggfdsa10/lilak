
#ifndef STDSiArray_HH
#define STDSiArray_HH

#include "LKDetectorPlane.h"
#include "LKLogger.h"

#include <map>
#include <tuple>
#include <vector>
#include "TH2Poly.h"

#include "STDPadPlane.h"
#include "GETChannel.h"
#include "LKMCTag.h"

class STDSiArray : public LKDetectorPlane
{
    public:
        STDSiArray();
        virtual ~STDSiArray() { ; }

        bool Init();
        void Clear(Option_t *option="");
        void Print(Option_t *option="") const;

        bool SetDataFromBranch(){return true;}
        void FillDataToHist(){;}

        bool IsInBoundary(Double_t x, Double_t z); // coordinate on the pad plane surface 

        Int_t FindSiDetID(Double_t x, Double_t z);
        Int_t FindUnitPadID(Double_t x, Double_t z);

        Int_t GetStripID4PadID(int unitPadID);
        Int_t GetStripID4JuncID(int juncID);
        Int_t GetOhmicID(int unitPadID);
        Int_t GetUnitPadID(int ohmicID, int juncID);

        Int_t GetSiDetID(int aget, int chan);
        Int_t GetOhmicID(int aget, int chan);
        Int_t GetJuncID(int aget, int chan);
        Int_t GetStripID(int aget, int chan);

        Int_t GetAsAdID(int unitPadID);
        Int_t GetAGETID(int siDetID, bool isOhmic=true);
        Int_t GetChanID4Ohmic(int siDetID, int ohmicID);
        Int_t GetChanID4Strip(int siDetID, int stripID, bool isFirstPairID);
        Int_t GetChannelIdx(int aget, int chan);

        // Position of the center of unit pads in PadPlane coordinate
        Double_t GetCenterUnitPadX(int siDetID, int unitPadID);
        Double_t GetCenterUnitPadX(int siDetID, int strip, int ohmic);
        Double_t GetCenterUnitPadZ(int siDetID, int unitPadID);
        Double_t GetCenterUnitPadZ(int siDetID, int strip, int ohmic);

        void ConvertSiLocalPos2Pad(int siDetID, double& x, double& y, double& z); // Convert position from Si-local coordinate (with respect to J1 channel coord.) to PadPlane coordinate
        void ConvertPad2SiLocalPos(int siDetID, double& x, double& y, double& z); // Convert position from PadPlane coordinate to Si-local coordinate (with respect to J1 channel coord.)

        Int_t GetFPNChannelID(int chan);
        bool IsFPNChannel(int chan);

        Int_t GetAsAdNum(){return fAsAdNum;}
        Int_t GetAGETNum(){return fAGETNum;}
        Int_t GetChanNum(){return fChanNum;}
        
        Int_t GetSiNum(){return fSiDetNum;}
        Int_t GetSiOhmicNum(){return fOhmicNum;}
        Int_t GetSiJuncNum(){return fJuncNum;}
        Int_t GetSiStripNum(){return fStripNum;}

        Double_t GetSiWidth(){return fSiWidth;}
        Double_t GetSiHeight(){return fSiHeight;}
        Double_t GetSiThickness(){return fSiThickness;}
        Double_t GetSiArrayPlaneDistAtPadCenter(){return fSiArrayPlaneDistAtPadCenter;}
        
        Double_t GetSiCenterPos(int detIdx, int xzIdx); // local padplane coordinate
        TString GetSiName(int detIdx);

        TH2Poly* GetSiPoly(int detIdx);

        LKMCTag* GetMCTag(int idx){return (LKMCTag*)fMCTagArray->At(idx);}

    private:
        void InitSiArrayGeometry();
        void InitChannelMapping();
        void InitChannelArray();

        // ====== Si-Array Parameters ======
        const int fAsAdNum = 1;
        const int fAGETNum = 4;
        const int fChanNum = 68;

        static const int fSiDetNum = 8;
        static const int fOhmicNum = 4;
        static const int fJuncNum = 16;
        static const int fStripNum = fJuncNum/2;

        const double fSiWidth = 40.3; // Si active width
        const double fSiHeight = 75.; // Si active height
        const double fSiThickness = 1.; // Si active thickness

        const double fSiArrayPlaneDistAtPadCenter = 133.; // Si detectors x-z position with respect to pad plane center

        double fSiDetectorCenter[fSiDetNum][2]; // [si detector num][x, z] in local pad coordinate
        TString fSiDetectorName[fSiDetNum];

        // ====== Si-Array Channel mapping structure ======
        map<pair<int, int>, pair<double, double>> fUnitPadPosMap_so[fSiDetNum]; // UnitPad position map using strip and ohmic
        map<int, pair<double, double>> fUnitPadPosMap_UnitPadIdx[fSiDetNum]; // UnitPad position map using Unitad index
        map<pair<int, int>, pair<int, int> > fChannelMap; // channel mapping array, [AGET, Channel] to ohmic or junction

        TH2Poly* fSiArrayPoly[fSiDetNum]; 
        TObjArray* fMCTagArray = nullptr;

        STDPadPlane* fPadPlane;

    ClassDef(STDSiArray,1);
};

#endif
