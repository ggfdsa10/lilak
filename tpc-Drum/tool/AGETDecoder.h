#ifndef AGETDECODER_HH
#define AGETDECODER_HH

#include "TClonesArray.h"
#include "LKLogger.h"
#include "LKParameterContainer.h"
#include "LKRun.h"
#include "LKTask.h"
#include "LKPad.h"
#include "TPCDrum.h"
#include "STDPadPlane.h"

#include "SejongDAQFlow.h"
#include "AGETGrawFrame.h"


class AGETDecoder : public LKTask
{
    public:
        AGETDecoder();
        virtual ~AGETDecoder(){}

        bool Init();
        bool Clear();
        void Run(Long64_t numEvents=-1);
        bool EndOfRun();

        void SetRunFile(vector<TString> fileList);

        void SetEventNumber(int event); // set the limit of number of event by runs
        Int_t GetTotalEventNumber(); // get the number of event after running

    private:
        Int_t FileOpen(int asadIdx);
        Int_t CheckEvent(int asadIdx);

        Int_t ReadHeader(int asadIdx);
        Int_t ReadItem(int asadIdx);

        Int_t FillData();

        TPCDrum* fDetector = nullptr;
        STDPadPlane* fDetectorPlane = nullptr;

        int fEventIdx = 0;
        int fEventNum = -1;

        int fAsAdNum = 0;
        int fPadNum = 0;

        TClonesArray* fEventHeaderArray = nullptr;
        TClonesArray* fPadArray = nullptr;

        AGETGrawFrame* fDAQFrame = nullptr;
        LKPad* fPad = nullptr;

        DAQList fDAQList;
        std::ifstream fDAQFile[ASADNUM];

        HeaderFrame mHeader;
        ItemFrame mItem;

    ClassDef(AGETDecoder,1);
};

#endif
