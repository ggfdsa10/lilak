#include "STDTriggerAction.h"

STDTriggerAction::STDTriggerAction()
: G4UserRunAction(), fIsSetTrigger(false), fTriggerDetName("")
{
    fRunManager = (LKG4RunManager *) LKG4RunManager::GetRunManager();
    fPar = fRunManager -> GetParameterContainer();
    fDet = fRunManager -> GetSensitiveDetectors();

    InitTriggerSetup();
    InitStepData();
}

STDTriggerAction::STDTriggerAction(LKG4RunManager *man)
    : fRunManager(man), fIsSetTrigger(false), fTriggerDetName("")
{
}

void STDTriggerAction::BeginOfRunAction(const G4Run*)
{
}

void STDTriggerAction::EndOfRunAction(const G4Run*)
{
}

void STDTriggerAction::SetStep(int copyNo, int trackID, bool firstFlag, double x, double y, double z, double edep)
{
    int detIdx = GetTriggerDetIdx(copyNo);
    if(detIdx == -1){return;}

    int existTrackIdx = -1;
    for(int i=0; i<fTrigDetector[detIdx].GetSize(); i++){
        if(fTrigDetector[detIdx].trackID[i] == trackID){
            existTrackIdx = i;
            break;
        }
    }

    if(existTrackIdx == -1){
        fTrigDetector[detIdx].trackID.push_back(trackID);
        fTrigDetector[detIdx].hitX.push_back(x);
        fTrigDetector[detIdx].hitY.push_back(y);
        fTrigDetector[detIdx].hitZ.push_back(z);
        fTrigDetector[detIdx].edep.push_back(edep);
    }
    else{
        if(firstFlag){
            fTrigDetector[detIdx].hitX[existTrackIdx] = x;
            fTrigDetector[detIdx].hitY[existTrackIdx] = y;
            fTrigDetector[detIdx].hitZ[existTrackIdx] = z;
        }
        fTrigDetector[detIdx].edep[existTrackIdx] += edep;
    }
}

bool STDTriggerAction::IsTriggeredEvent()
{
    int detNum = fTrigDetector.size();

    for(int det=0; det<detNum; det++){
        double de = 0.;
        for(int i=0; i<fTrigDetector[detIdx].GetSize(); i++){
            double edep = fTrigDetector[detIdx].edep[i];
            de += edep;
        }
        if(de > 0.1){return true;}
    }
    return false;
}

void STDTriggerAction::InitTriggerSetup()
{
    cout << "STDTriggerAction::InitTriggerSetup() " << endl;
    
    if(fPar -> CheckPar("Trigger/Detector/Name")){
        fTriggerDetName = fPar -> GetParString("Trigger/Detector/Name");
    }
    if(fPar -> CheckPar("Trigger/Detector/EnergyThreshold")){
        fTriggerEThreshold = fPar -> GetParVDouble("Trigger/Detector/EnergyThreshold");
    }

    int logicIdx = 1;
    while(1){
        if(!fPar -> CheckPar(Form("Trigger/Logic%i", logicIdx))){break;}

        cout << Form("Trigger/Logic%i", logicIdx)  << endl;
        vector<TString> tmp = fPar -> GetParVString(Form("Trigger/Logic%i", logicIdx));
        if(tmp.size() != 3){continue;}
        tmp[0].ToUpper();
        tmp[1].ToUpper();
        tmp[2].ToUpper();

        int index[2];
        for(int lr=0; lr<2; lr++){
            int leftRight = (lr==0)? 0 : 2;
            if(!tmp[leftRight].IsDigit()){
                for(auto i=0; i<fTrigDetName.size(); i++){
                    if(fTrigDetName[i].Index(tmp[leftRight]) != -1){
                        index[lr] = i;
                        break;
                    }
                }
            }
            else{
                index[lr] = GetTriggerDetIdx(tmp[leftRight].Atoi());
            }
            if(index[lr] < 0){return;}
        }

        if(tmp[1].Index("OR") != -1){
            cout << index[0] << " OR " << index[1] << endl;
            // fTriggerLogic.OR.push_back(make_pair(index[0], index[1]));
        }
        if(tmp[1].Index("AND") != -1){
            cout << index[0] << " AND " << index[1] << endl;
            // fTriggerLogic.AND.push_back(make_pair(index[0], index[1]));
        }
        logicIdx++;
    }

    if(fTriggerEThreshold.size() != 0){fIsSetTrigger = true;}
}

void STDTriggerAction::InitStepData()
{
    TIter itDetectors(fDet);
    LKParameter *parameter;
    while ((parameter = dynamic_cast<LKParameter*>(itDetectors())))
    {
        TString detName = parameter -> GetName();
        Int_t copyNo = parameter -> GetInt();

        if(fTriggerDetName.IsNull()){
            fTrigDetCopyNo.push_back(copyNo);
            fTrigDetName.push_back(detName);
        }
        else{
            if(detName.Index(fTriggerDetName) != -1){
                fTrigDetCopyNo.push_back(copyNo);
                fTrigDetName.push_back(detName);
            }
        }
    }

    fTrigDetector.resize(fTrigDetCopyNo.size());
    for(int i=0; i<fTrigDetCopyNo.size(); i++){
        fTrigDetector[i].Clear();
    }
}

int STDTriggerAction::GetTriggerDetIdx(int copyNo)
{
    for(auto i=0; i<fTrigDetCopyNo.size(); i++){
        if(copyNo == fTrigDetCopyNo[i]){return i;}
    }
    return -1;
}