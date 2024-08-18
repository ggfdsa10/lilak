#ifndef AGETGRAWFRAME_HH
#define AGETGRAWFRAME_HH

#include <iostream>
#include <string.h>

using namespace std;

#define nAGET 4       // The number of AGETs
#define nCHAN 68      // The number of AGET's channels
#define nBUCK 512     // The number of channel's timebuckets
#define nADCs 139264  // The number of ADC(12 bits); nAGET * nCHAN * nBUCK = 139264

typedef struct HeaderFrame{
    uint8_t metaType;       // 1
    uint8_t frameSize[3];   // 4
    uint8_t dataSource;     // 5
    uint8_t frameType[2];   // 7
    uint8_t revision;       // 8
    uint8_t headerSize[2];  // 10
    uint8_t itemSize[2];    // 12
    uint8_t nItems[4];      // 16
    uint8_t eventTime[6];   // 22
    uint8_t eventId[4];     // 26
    uint8_t coboId;         // 27
    uint8_t asadId;         // 28
    uint8_t readOffset[2];  // 30
    uint8_t status;         // 31
    uint8_t hitPat_0[9];    // 40
    uint8_t hitPat_1[9];    // 49
    uint8_t hitPat_2[9];    // 58
    uint8_t hitPat_3[9];    // 67
    uint8_t multip_0[2];    // 69
    uint8_t multip_1[2];    // 71
    uint8_t multip_2[2];    // 73
    uint8_t multip_3[2];    // 75
    uint8_t windowOut[4];   // 79
    uint8_t lastCell_0[2];  // 81
    uint8_t lastCell_1[2];  // 83
    uint8_t lastCell_2[2];  // 85
    uint8_t lastCell_3[2];  // 87 bytes
}HeaderFrame;

typedef struct ItemFrame{
    uint32_t items;
}ItemFrame;

class AGETGrawFrame
{
    public:
        AGETGrawFrame();
        ~AGETGrawFrame();

        void Clear();

        int mMetaType; // [bytes]
        int mFrameSize;
        int mDataSource;
        int mFrameType; // 1: partial readout or zero suppressed mode, 2: full readout mode
        int mRevision;
        int mHeaderSize;// [bytes]
        int mItemSize; // [bytes]
        int mNitems;        
        int mEventID;    
        int mPrivEventID;        
        int mCoboID;             
        int mAsadID;             
        int mReadOffset;         
        int mStatus;             
        int mMultip[nAGET];      
        int mWindowOut;          
        int mLastCell[nAGET];    
        bool mIsHit[nAGET][72];  
        uint64_t mEventTime; // [10 ns]
        uint64_t mDiffTime; // [10 ns]
        uint64_t mPrevTime; // [10 ns]

        uint16_t mADC[nAGET][nCHAN][nBUCK];
};

#endif