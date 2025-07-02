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
    fRunManager -> NextEvent();
}