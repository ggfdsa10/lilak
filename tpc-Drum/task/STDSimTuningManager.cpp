#include "STDSimTuningManager.h"

ClassImp(STDSimTuningManager);

STDSimTuningManager::STDSimTuningManager()
{
    fName = "STDSimTuningManager";
}

bool STDSimTuningManager::Init()
{
    return true;
}

void STDSimTuningManager::Exec(Option_t *option)
{
}

bool STDSimTuningManager::EndOfRun()
{
    return true;
}