#include "LKG4RunManager.h"
#include "FTFP_BERT_HP.hh"

// Geant4
#include "TPCDrumConstruction.h"

// NPTool
#include "NPOptionManager.h"
#include "PhysicsList.hh"

int main(int argc, char** argv)
{
    auto runManager = new LKG4RunManager();

    // runManager -> AddParameterContainer(argv[1]);
    // runManager -> Run(argc, argv);

    NPOptionManager* NPOptionManager = NPOptionManager::getInstance("--random-seed 0"); // <= option will be inserted as -E, -R... 
    NPOptionManager->SetIsSimulation();
    NPOptionManager->SetReactionFile("./test.reaction");

    TPCDrumConstruction* detector = new TPCDrumConstruction();
    runManager -> SetUserInitialization(detector);

    PhysicsList* NPPhysicsList = new PhysicsList();
    runManager -> SetUserInitialization(NPPhysicsList);

    cout << NPOptionManager->GetReactionFile() << endl;

    return 0;
}
