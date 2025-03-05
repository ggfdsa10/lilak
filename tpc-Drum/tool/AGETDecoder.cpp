#include "AGETDecoder.h"

ClassImp(AGETDecoder);

AGETDecoder::AGETDecoder()
: fEventIdx(0)
{
    fName = "AGETDecoder";
}

bool AGETDecoder::Init()
{
    lk_info << "Initializing AGETDecoder" << std::endl;

    fDetector = (TPCDrum *) fRun -> GetDetector();
    fDetectorPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();

    fEventHeaderArray = fRun -> RegisterBranchA("FrameHeader", "LKEventHeader", 1);

    int tpc_asadNum = fDetectorPlane -> GetAsAdNum();
    int tpc_AGETNum = fDetectorPlane -> GetAGETNum();
    fChannelArray = fRun -> RegisterBranchA("RawPad", "GETChannel", tpc_asadNum*tpc_AGETNum);

    if(!fDAQFrame){fDAQFrame = new AGETGrawFrame[ASADNUM];}

    Clear();

    return true;
}

bool AGETDecoder::Clear()
{
    for(int i=0; i<ASADNUM; i++){
        if(fDAQFile[i].is_open()){fDAQFile[i].close();}

        if(fDAQFrame){
            fDAQFrame[i].Clear();
            fDAQFrame[i].mPrevTime = 0;
        }
    }
    fDAQList.clear();

    return true;
}

void AGETDecoder::Run(Long64_t numEvents)
{
    fAsAdNum = 0;
    for(int i=0; i<ASADNUM; i++){
        bool isEmpty = fDAQList[i].empty();
        if(!isEmpty){fAsAdNum++;}
    }

    bool isEndOfRun = false;
    while(1){
        for(int i=0; i<fAsAdNum; i++){
            if(!FileOpen(i)){
                if(i==0){isEndOfRun = true;}
                continue;
            }

            fDAQFrame[i].Clear();
            
            ReadHeader(i);
            ReadItem(i);

            if(!CheckEvent(i)){isEndOfRun = true;}
        }
        if(isEndOfRun){
            fRun -> EndOfRun();
            break;
        }
        
        FillData();
        fRun -> ExecuteNextEvent();

        if(fEventNum != -1){
            if(fRun->GetCurrentEventID()==fEventNum){break;}
        }
        fEventIdx++;
    }
}

bool AGETDecoder::EndOfRun()
{
    return true;
}

void AGETDecoder::SetRunFile(vector<TString> fileList)
{
    fDAQList = SejongDAQFlow::GetDAQList(fileList);
}

void AGETDecoder::SetEventNumber(int event){fEventNum = event;}
Int_t AGETDecoder::GetTotalEventNumber(){return fEventIdx;}

Int_t AGETDecoder::FileOpen(int asadIdx)
{
    if(!fDAQList[asadIdx].empty() && !fDAQFile[asadIdx].is_open()){
        fDAQFile[asadIdx].open(fDAQList[asadIdx].front().Data(), std::ios::binary);

        if(!fDAQFile[asadIdx].is_open()){
            cout << "AGETDecoder::FileOpen() --- Error: " << fDAQList[asadIdx].front().Data() << " can not open !!!" << endl;
            cout << "AGETDecoder::FileOpen() --- " << fDAQList[asadIdx].front().Data() << " to be skipped..." << endl;
            fDAQFile[asadIdx].close();
            fDAQList[asadIdx].pop();
            FileOpen(asadIdx);

            return 1;
        }
        fDAQList[asadIdx].pop();

        return 1;
    }
    if(fDAQList[asadIdx].empty() && fDAQFile[asadIdx].eof()){
        fDAQFile[asadIdx].close();
        return 0;
    }
    if(!fDAQList[asadIdx].empty() && fDAQFile[asadIdx].eof()){
        fDAQFile[asadIdx].close();
        FileOpen(asadIdx);
        return 1;
    }
    return 1;
}


Int_t AGETDecoder::CheckEvent(int asadIdx)
{
    if(fDAQFile[asadIdx].eof()){
        if(!FileOpen(asadIdx)){return 0;}
        fDAQFrame[asadIdx].Clear();
        ReadHeader(asadIdx);
        ReadItem(asadIdx);
    }
    
    return 1;
}

Int_t AGETDecoder::ReadHeader(int asadIdx)
{
    fDAQFile[asadIdx].read((char*)&(mHeader), sizeof(mHeader));

    fDAQFrame[asadIdx].mMetaType = (int)pow(2, (int)mHeader.metaType);
    fDAQFrame[asadIdx].mFrameSize = (int)((mHeader.frameSize[0] << 16) | (mHeader.frameSize[1] << 8) | (mHeader.frameSize[2]));
    fDAQFrame[asadIdx].mDataSource = (int)mHeader.dataSource;
    fDAQFrame[asadIdx].mFrameType = (int)((mHeader.frameType[0] << 8) | mHeader.frameType[1]);
    fDAQFrame[asadIdx].mRevision = (int)mHeader.revision;
    fDAQFrame[asadIdx].mHeaderSize = (int)((mHeader.headerSize[0] << 8) | mHeader.headerSize[1]) * fDAQFrame[asadIdx].mMetaType;
    fDAQFrame[asadIdx].mItemSize = (int)((mHeader.itemSize[0] << 8) | mHeader.itemSize[1]);
    fDAQFrame[asadIdx].mNitems = (int)((mHeader.nItems[0] << 24) | (mHeader.nItems[1] << 16) | (mHeader.nItems[2] << 8) | (mHeader.nItems[3]));
    fDAQFrame[asadIdx].mEventTime = (((uint64_t)mHeader.eventTime[0] << 40) | ((uint64_t)mHeader.eventTime[1] << 32) |
                             ((uint64_t)mHeader.eventTime[2] << 24) | ((uint64_t)mHeader.eventTime[3] << 16) |
                             ((uint64_t)mHeader.eventTime[4] << 8) | ((uint64_t)mHeader.eventTime[5]));

    fDAQFrame[asadIdx].mEventID = ((mHeader.eventId[0] << 24) | (mHeader.eventId[1] << 16) | (mHeader.eventId[2] << 8) | (mHeader.eventId[3]));
    fDAQFrame[asadIdx].mCoboID = (int)mHeader.coboId;
    fDAQFrame[asadIdx].mAsadID = (int)mHeader.asadId;
    fDAQFrame[asadIdx].mReadOffset = (int)((mHeader.readOffset[0] << 8) | mHeader.readOffset[1]);
    fDAQFrame[asadIdx].mStatus = (int)mHeader.status;

    fDAQFrame[asadIdx].mDiffTime = fDAQFrame[asadIdx].mEventTime - fDAQFrame[asadIdx].mPrevTime;
    if(fDAQFrame[asadIdx].mEventID == 0){fDAQFrame[asadIdx].mDiffTime = 0;}
    fDAQFrame[asadIdx].mPrevTime = fDAQFrame[asadIdx].mEventTime;

    int chanID = 0;
    for(int i=0; i<9; i++){
        for(int shift=7; shift>=0; shift--){
            if (chanID == 0) shift = 3;
            fDAQFrame[asadIdx].mIsHit[0][chanID] = mHeader.hitPat_0[i] & (1 << shift) ? true : false;
            fDAQFrame[asadIdx].mIsHit[1][chanID] = mHeader.hitPat_1[i] & (1 << shift) ? true : false;
            fDAQFrame[asadIdx].mIsHit[2][chanID] = mHeader.hitPat_2[i] & (1 << shift) ? true : false;
            fDAQFrame[asadIdx].mIsHit[3][chanID] = mHeader.hitPat_3[i] & (1 << shift) ? true : false;
            chanID++;
        }
    }

    uint8_t waste;
    for(int i=0; i<(fDAQFrame[asadIdx].mHeaderSize - sizeof(mHeader)); i++){
        fDAQFile[asadIdx].read((char*)&waste, sizeof(uint8_t));
    }

    return 1;
}

Int_t AGETDecoder::ReadItem(int asadIdx)
{
    if(fDAQFrame[asadIdx].mFrameType == 1){
        // Partial readout mode
        // Frame Item Format:     aacc cccc | cbbb bbbb | bb00 ssss | ssss ssss
        // Read in reverse order: ssss ssss | bb00 ssss | cbbb bbbb | aacc cccc
        // a: agetID, c: chanID, b: buckID, s: sample, 0: empty

        int agetID, chanID, buckID, sample;
        int chanIDLowerBits, chanIDUpperBits;
        int buckIDLowerBits, buckIDUpperBits;
        int sampleLowerBits, sampleUpperBits;

        for(int itemId=0; itemId < fDAQFrame[asadIdx].mNitems; itemId++) {
            fDAQFile[asadIdx].read((char*)&(mItem), sizeof(uint32_t));

            // Read agetID
            agetID = (mItem.items >> 6) & 0x03;

            // Read chanID
            chanIDLowerBits = (mItem.items >> 15) & 0x01;
            chanIDUpperBits = mItem.items & 0x3f;
            chanID = (chanIDUpperBits << 1) | chanIDLowerBits;

            // Read buckID
            buckIDLowerBits = (mItem.items >> 22) & 0x03;
            buckIDUpperBits = (mItem.items >> 8) & 0x7f;
            buckID = (buckIDUpperBits << 2) | buckIDLowerBits;

            // Read sample
            sampleLowerBits = (mItem.items >> 24) & 0xff;
            sampleUpperBits = (mItem.items >> 16) & 0x0f;
            sample = (sampleUpperBits << 8) | sampleLowerBits;
            fDAQFrame[asadIdx].mADC[agetID][chanID][buckID] = sample;
        }
    } 
    else if(fDAQFrame[asadIdx].mFrameType == 2){
        // Full readout mode
        // Frame Item Format:     aa00 ssss | ssss ssss | aa00 ssss | ssss ssss
        // Read in reverse order: ssss ssss | aa00 ssss | ssss ssss | aa00 ssss
        // a: agetID, s: sample, 0: empty

        int sample, sampleLowerBits, sampleUpperBits;

        for(int buckID=0; buckID<nBUCK; buckID++){
            for (int chanID=0; chanID<nCHAN; chanID += 2){
                for (int agetID=0; agetID<nAGET; agetID++){
                    fDAQFile[asadIdx].read((char *)&(mItem), sizeof(uint32_t));

                    // Read sample 1
                    sampleLowerBits = (mItem.items >> 8) & 0xff;
                    sampleUpperBits = mItem.items & 0x0f;
                    sample = (sampleUpperBits << 8) | sampleLowerBits;
                    fDAQFrame[asadIdx].mADC[agetID][chanID][buckID] = sample;
                    
                    // Read sample 2
                    sampleLowerBits = (mItem.items >> 24) & 0xff;
                    sampleUpperBits = (mItem.items >> 16) & 0x0f;
                    sample = (sampleUpperBits << 8) | sampleLowerBits;
                    fDAQFrame[asadIdx].mADC[agetID][chanID + 1][buckID] = sample;
                }
            }
        }
    }

    return 1;
}

Int_t AGETDecoder::FillData()
{
    fChannelArray -> Clear("C");
    for(int asad=0; asad<fAsAdNum; asad++){
        for(int aget=0; aget<AGETNUM; aget++){
            for(int chan=0; chan<CHANNUM; chan++){
                if(fDetectorPlane->IsFPNChannel(chan)){continue;}
                if(fDAQFrame[asad].mIsHit[aget][chan] == false){continue;} // we don't saved it, if it is not hitted channel

                int layer = fDetectorPlane -> GetLayerID(asad, aget, chan);
                int row = fDetectorPlane -> GetRowID(asad, aget, chan);
                int padID = fDetectorPlane -> GetPadID(layer, row);

                fChannel = (GETChannel*)fChannelArray -> ConstructedAt(padID);
                int fpnChannel = fDetectorPlane->GetFPNChannelID(chan);

                // Subtract the FPN signal
                int ADC[TIMEBUCKET];
                for(int tb=0; tb<TIMEBUCKET; tb++){ 
                    int adc = fDAQFrame[asad].mADC[aget][chan][tb];
                    int fpn = fDAQFrame[asad].mADC[aget][fpnChannel][tb];
                    ADC[tb] = adc - fpn;
                }

                fChannel -> SetAsad(asad);
                fChannel -> SetAget(aget);
                fChannel -> SetChan(chan);
                fChannel -> SetPadID(padID);

                fChannel -> SetWaveformY(ADC);
            }
        }
    }
    return 1;
}

