#include "LKG4RunManager.h"
#include "LKParameterContainer.h"

#include <time.h>
#include "globals.hh"
#include "Randomize.hh"

// Geant4 
#include "G4UImanager.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4LossTableManager.hh"
#include "G4EmConfigurator.hh"
#include "G4PAIModel.hh"
#include "G4Electron.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

// LILAK
#include "TPCDrumConstruction.h"

// NPTool
#include "NPOptionManager.h"
#include "RootOutput.h"
#include "PrimaryGeneratorAction.hh"
#include "PhysicsList.hh"
#include "LKPrimaryGeneratorAction.h"

int main(int argc, char** argv)
{
    auto runManager = new LKG4RunManager();
    runManager -> AddParameterContainer(argv[1]);
    auto par = runManager -> GetParameterContainer();

    TString reactionFile = par->GetParString("NPTool/ReactionFile");
    NPOptionManager::getInstance()->SetIsSimulation();
    NPOptionManager::getInstance()->SetReactionFile(reactionFile.Data());

    gRandom->SetSeed(time(0));
    CLHEP::HepRandom::setTheSeed(time(0), 3);
    G4Random::setTheSeed(time(0));
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    
    TPCDrumConstruction* detector = new TPCDrumConstruction();
    runManager -> SetUserInitialization(detector);

    PhysicsList* NPPhysicsList = new PhysicsList();
    runManager -> SetUserInitialization(NPPhysicsList);

    runManager -> Initialize();

    PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(detector);
    primary->ReadEventGeneratorFile(reactionFile.Data());
    runManager->SetUserAction(primary);

    runManager -> Run(argc, argv);

    return 0;
}
