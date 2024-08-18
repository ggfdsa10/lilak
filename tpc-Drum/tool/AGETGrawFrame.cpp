#include "AGETGrawFrame.h"

AGETGrawFrame::AGETGrawFrame()
{
}

AGETGrawFrame::~AGETGrawFrame()
{
}

void AGETGrawFrame::Clear()
{
    mMetaType = -1; 
    mFrameSize = -1;
    mDataSource = -1;
    mFrameType = -1;
    mRevision = -1;
    mHeaderSize = -1;
    mItemSize = -1; 
    mNitems = -1;        
    mEventID = -1;            
    mCoboID = -1;             
    mAsadID = -1;                 
    mReadOffset = -1;             
    mStatus = -1;     
    mWindowOut = -1;   
    memset(mMultip, 0, sizeof(mMultip));  
    memset(mLastCell, 0, sizeof(mLastCell));  
    memset(mIsHit, 0, sizeof(mIsHit));  
    memset(mADC, 0, sizeof(mADC));  
    mEventTime = 0;
    mDiffTime = 0;
}