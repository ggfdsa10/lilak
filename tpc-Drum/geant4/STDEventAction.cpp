#include "STDEventAction.h"

#include "G4Event.hh"
#include "G4RunManager.hh"

STDEventAction::STDEventAction()
: G4UserEventAction()
{
    fRunManager = (LKG4RunManager *) LKG4RunManager::GetRunManager();
}

STDEventAction::STDEventAction(LKG4RunManager *man)
: G4UserEventAction(), fRunManager(man)
{
}

void STDEventAction::EndOfEventAction(const G4Event*)
{
    fTriggerAction = (STDTriggerAction*)LKG4RunManager::GetRunManager() -> GetUserRunAction();
    bool isFire = fTriggerAction -> IsTriggeredEvent();

    if(isFire){
        fRunManager -> NextEvent();
    }
    else{
        fRunManager -> ClearEvent();
    }
}