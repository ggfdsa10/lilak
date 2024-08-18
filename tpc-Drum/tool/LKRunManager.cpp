#include "LKRunManager.h"

ClassImp(LKRunManager);

LKRunManager::LKRunManager()
: fIsDAQStage(false), fInputRun(""), fRejectRun(""), fTotalEventNum(-1), fEventNumByRun(-1), fCurrentEventIdx(0)
{
}

bool LKRunManager::Init()
{
    if(fInputRun != ""){
        fRunList = SejongDAQFlow::GetRunList(fInputRun, fRejectRun, fIsDAQStage);

        if(fIsDAQStage){
            fDecoder = new AGETDecoder();
            LKRun::SetEventTrigger(fDecoder);
        }
    }
    else{lk_error << "LKRunManager::Init() There is no set the Input Run List!!!" << endl;}
    LKRun::SetAutoTermination(false);

    return true;
}

bool LKRunManager::Run()
{
    if(fIsDAQStage){
        for(int run=0; run<fRunList.size(); run++){
            cout << "test start of run" << endl;
            LKRun::SetOutputFile(Form("./test_%i.root", fRunList[run].first));
            LKRun::Init();

            cout << "test " << endl;

            // for AGET Decoder
            fDecoder -> Clear();
            if(fTotalEventNum != -1){fDecoder -> SetEventNumber(fTotalEventNum-fCurrentEventIdx-1);}
            if(fEventNumByRun != -1){fDecoder -> SetEventNumber(fEventNumByRun-1);}
            fDecoder -> SetRunFile(fRunList[run].second);
        
            // Main Run process
            LKRun::Run();

            if(fTotalEventNum != -1){fCurrentEventIdx += fDecoder -> GetTotalEventNumber();}
        }
    }
    else{
        cout << fInputRun << endl;
        LKRun::SetDataPath(fDataPath);
        LKRun::AddInputList(fInputRun);
        LKRun::Init();
        LKRun::Run();
    }

    LKRun::Terminate(this);
    return true;
}

bool LKRunManager::EndOfRun()
{
    return true;
}

void LKRunManager::SetDatapath(TString path){fDataPath = path;}
void LKRunManager::SetRunList(TString list){fInputRun = list;}
void LKRunManager::SetRejectRun(TString list){fRejectRun = list;}

void LKRunManager::SetEventNumber(int event){fTotalEventNum = event;}
void LKRunManager::SetEventNumByRun(int event){fEventNumByRun = event;}

void LKRunManager::SetDAQStage(){fIsDAQStage = true;}
