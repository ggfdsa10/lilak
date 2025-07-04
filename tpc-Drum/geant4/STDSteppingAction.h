#ifndef STDSteppingAction_HH
#define STDSteppingAction_HH


#include "LKG4RunManager.h"
#include "LKParameterContainer.h"
#include "LKParameter.h"
#include "LKSteppingAction.h"
#include "G4Step.hh"
#include "globals.hh"
#include "STDTriggerAction.h"

class STDSteppingAction : public LKSteppingAction
{
    public:
        STDSteppingAction();
        STDSteppingAction(LKG4RunManager *man);
        virtual ~STDSteppingAction() {}

        virtual void UserSteppingAction(const G4Step*);

    private:
        LKG4RunManager *fRunManager = nullptr;
        LKParameterContainer* fPar = nullptr;

        STDTriggerAction* fTriggerAction = nullptr;
};

#endif