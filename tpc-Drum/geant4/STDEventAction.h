#ifndef STDEventAction_HH
#define STDEventAction_HH

#include "LKG4RunManager.h"
#include "G4UserEventAction.hh"
#include "G4Event.hh"

class STDEventAction : public G4UserEventAction
{
    public:
        STDEventAction();
        STDEventAction(LKG4RunManager *man);
        virtual ~STDEventAction() {}

        virtual void EndOfEventAction(const G4Event* event);

    private:
        LKG4RunManager *fRunManager = nullptr;
};

#endif