#ifndef STDTriggerAction_HH
#define STDTriggerAction_HH

#include <vector>

#include "LKG4RunManager.h"
#include "LKParameterContainer.h"
#include "LKParameter.h"

#include "G4UserRunAction.hh"
#include "G4Run.hh"

class STDTriggerAction : public G4UserRunAction
{
    struct TrigDetData{
        vector<int> trackID;
        vector<double> hitX;
        vector<double> hitY;
        vector<double> hitZ;
        vector<double> edep;

        void Clear(){
            trackID.clear();
            hitX.clear();
            hitY.clear();
            hitZ.clear();
            edep.clear();
        }
        int GetSize(){return trackID.size();}
    };

    struct TrigLogic{
        vector<pair<int, int>> OR;
        vector<pair<int, int>> AND;
    };

    public:
        STDTriggerAction();
        STDTriggerAction(LKG4RunManager *man);
        virtual ~STDTriggerAction() {}

        virtual void BeginOfRunAction(const G4Run*);
        virtual void EndOfRunAction(const G4Run*);

        void SetStep(int copyNo, int trackID, bool firstFlag, double x, double y, double z, double edep);
        bool IsTriggeredEvent();

    private:
        void InitTriggerSetup();
        void InitStepData();
        int GetTriggerDetIdx(int copyNo);

        LKG4RunManager *fRunManager = nullptr;
        LKParameterContainer* fPar = nullptr;
        LKParameterContainer* fDet = nullptr;

        bool fIsSetTrigger;
        TString fTriggerDetName;
        vector<double> fTriggerEThreshold;
        vector<TrigLogic> fTriggerLogic;

        vector<int> fTrigDetCopyNo;
        vector<TString> fTrigDetName;
        vector<TrigDetData> fTrigDetector;
};

#endif