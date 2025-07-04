#include "STDSteppingAction.h"

#include "G4Event.hh"
#include "G4RunManager.hh"

STDSteppingAction::STDSteppingAction()
: LKSteppingAction()
{
    fRunManager = (LKG4RunManager *) LKG4RunManager::GetRunManager();
    fPar = fRunManager -> GetSensitiveDetectors();
}

STDSteppingAction::STDSteppingAction(LKG4RunManager *man)
    : LKSteppingAction(man), fRunManager(man)
{
}

void STDSteppingAction::UserSteppingAction(const G4Step* step)
{   
    G4int preNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
    TString volumeName = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetName();
   

    if(volumeName.Index("SiDet") != -1){

        // cout << preNo << " " << endl;
        
        bool isFirstStep = step -> IsFirstStepInVolume();
        G4int trackID = step -> GetTrack() -> GetTrackID();
        G4ThreeVector stepPos = .5 * (step -> GetPreStepPoint() -> GetPosition() + step -> GetPostStepPoint() -> GetPosition());
        G4double edep = step -> GetTotalEnergyDeposit(); 

        fTriggerAction = (STDTriggerAction*)LKG4RunManager::GetRunManager() -> GetUserRunAction();
        fTriggerAction -> SetStep(preNo, trackID, isFirstStep, stepPos.x(), stepPos.y(), stepPos.z(), edep);
    }

    LKSteppingAction::UserSteppingAction(step);
}