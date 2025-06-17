#include "STDDriftElectronMaker.h"

ClassImp(STDDriftElectronMaker);

STDDriftElectronMaker::STDDriftElectronMaker()
{
    fName = "STDDriftElectronMaker";
}

bool STDDriftElectronMaker::Init()
{
    fPar = fRun -> GetParameterContainer();
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();

    int tpc_asadNum = fPadPlane -> GetAsAdNum();
    int tpc_AGETNum = fPadPlane -> GetAGETNum();
    fChannelArray = fRun -> RegisterBranchA("RawPad", "GETChannel", tpc_asadNum*tpc_AGETNum);
    fTrackArray = fRun -> GetBranchA("MCTrack");
    fStepArray = fRun -> GetBranchA("MCStepTPCDrum");

    return true;
}

void STDDriftElectronMaker::Exec(Option_t *option)
{
    cout << fTrackArray -> GetEntries() << endl;
    cout << fStepArray -> GetEntries() << endl;

    
}

bool STDDriftElectronMaker::EndOfRun()
{
    return true;
}