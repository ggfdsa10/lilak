#include "STDDecoder.h"

ClassImp(STDDecoder);

STDDecoder::STDDecoder()
: fEventIdx(0)
{
    fName = "STDDecoder";
}

bool STDDecoder::Init()
{
    lk_info << "Initializing STDDecoder" << std::endl;

    fDetector = (TPCDrum *) fRun -> GetDetector();
    fDetectorPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();

    fEventHeaderArray = fRun -> RegisterBranchA("FrameHeader", "LKEventHeader", 1);
    fChannelArray = fRun -> RegisterBranchA("RawPad", "GETChannel", ASADNUM*AGETNUM*CHANNUM);

    if(!fDecoder){fDecoder = new GETDecoder[ASADNUM];}

    Clear();

    return true;
}

bool STDDecoder::Clear()
{
    fDAQList.clear();
    memset(fAsAdOn, false, sizeof(fAsAdOn));

    return true;
}

void STDDecoder::Run(Long64_t numEvents)
{
    AddFiles();

    if(fEventNum != -1){fEventIdx = fEventNum;}
    for(int event=0; event<fEventIdx; event++){
        for(int asad=0; asad<ASADNUM; asad++){
            if(!fAsAdOn[asad]){continue;}
            fFrame[asad] = fDecoder[asad].GetBasicFrame(event);
            fFrame[asad] -> Print(); // test
        }

        FillData();
        fRun -> ExecuteNextEvent();
    }
    fRun -> EndOfRun();
}

bool STDDecoder::EndOfRun()
{
    return true;
}

void STDDecoder::SetRunFile(vector<TString> fileList)
{
    fDAQList = SejongDAQFlow::GetDAQList(fileList);
}

void STDDecoder::SetEventNumber(int event){fEventNum = event;}
Int_t STDDecoder::GetTotalEventNumber(){return fEventIdx;}

Int_t STDDecoder::AddFiles()
{
    for(int i=0; i<ASADNUM; i++){
        bool isEmpty = fDAQList[i].empty();
        if(!isEmpty){
            while(!fDAQList[i].empty()){
                fDecoder[i].AddData(fDAQList[i].front());
                fDAQList[i].pop();
            }
        }
    }

    for(int asad=0;asad<ASADNUM; asad++){
        if(fDecoder[asad].GetNumData() == 0){continue;}
        fDecoder[asad].SetData(0);
        fDecoder[asad].GoToEnd();
        fEventIdx = fDecoder[asad].GetNumFrames();
        fAsAdOn[asad] = true;
    }

    return 1;
}

Int_t STDDecoder::FillData()
{
    fChannelArray -> Clear("C");
    int channelIdx = 0;
    for(int asad=0; asad<ASADNUM; asad++){
        if(!fAsAdOn[asad]){continue;}

        for(int aget=0; aget<AGETNUM; aget++){
            for(int chan=0; chan<CHANNUM; chan++){
                Int_t *sample = fFrame[asad] -> GetSample(aget, chan);
                if(fDetectorPlane->IsFPNChannel(chan)){continue;}

                fChannel = (GETChannel*)fChannelArray -> ConstructedAt(channelIdx);
                int fpnChannel = fDetectorPlane->GetFPNChannelID(chan);
                Int_t *fpnSample = fFrame[asad] -> GetSample(aget, fpnChannel);

                int ADC[TIMEBUCKET];
                double meanFPN = 0.;
                for(int tb=0; tb<TIMEBUCKET; tb++){ 
                    int adc = sample[tb];
                    int fpn = fpnSample[tb];
                    // ADC[tb] = adc - fpn;
                    ADC[tb] = adc;
                    meanFPN += double(fpn);
                }
                meanFPN /= double(TIMEBUCKET);

                fChannel -> SetAsad(asad);
                fChannel -> SetAget(aget);
                fChannel -> SetChan(chan);
                fChannel -> SetPedestal(meanFPN);
                fChannel -> SetWaveformY(ADC);
                channelIdx++;
            }
        }
    }

    return 1;
}

