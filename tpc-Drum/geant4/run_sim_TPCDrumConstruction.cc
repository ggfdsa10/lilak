/// This macro create geant4 simulation with TPCDrumConstruction
#include "LKLogger.h"
#include "LKG4RunManager.h"
#include "FTFP_BERT_HP.hh"
#include "G4StepLimiterPhysics.hh"
#include "TPCDrumConstruction.h"

int main(int argc, char** argv)
{
    lk_logger("data/log");

    auto runManager = new LKG4RunManager();
    G4VModularPhysicsList* physicsList = new FTFP_BERT_HP;
    physicsList -> RegisterPhysics(new G4StepLimiterPhysics());
    runManager -> SetUserInitialization(physicsList);
    runManager -> AddParameterContainer(argv[1]);
    runManager -> SetUserInitialization(new TPCDrumConstruction());
    runManager -> Run(argc, argv);

    delete runManager;

    return 0;
}
