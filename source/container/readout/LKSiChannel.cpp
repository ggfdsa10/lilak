#include "LKSiChannel.h"

ClassImp(LKSiChannel);

LKSiChannel::LKSiChannel()
{
    Clear();
}

void LKSiChannel::Clear(Option_t *option)
{
    GETChannel::Clear(option);
    fLocalID = -1;
    fSide = -1;
    fStrip = -1;
    fDirection = -1;
    fPairArrayIndex = -1;
    fPairChannel = nullptr; //!
    fPhi1 = -99.9;
    fPhi2 = -99.9;
    fTheta1 = -99.9;
    fTheta2 = -99.9;

}

void LKSiChannel::Copy(TObject &object) const
{
    GETChannel::Copy(object);
    auto objCopy = (LKSiChannel &) object;
    objCopy.SetSide(fSide);
    objCopy.SetStrip(fStrip);
    objCopy.SetDirection(fDirection);
    objCopy.SetPhi1(fPhi1);
    objCopy.SetPhi2(fPhi2);
    objCopy.SetTheta1(fTheta1);
    objCopy.SetTheta2(fTheta2);
}

TObject* LKSiChannel::Clone(const char *newname) const
{
    LKSiChannel *obj = (LKSiChannel*) GETChannel::Clone(newname);
    obj -> SetLocalID(fLocalID);
    obj -> SetSide(fSide);
    obj -> SetStrip(fStrip);
    obj -> SetDirection(fDirection);
    obj -> SetPhi1(fPhi1);
    obj -> SetPhi2(fPhi2);
    obj -> SetTheta1(fTheta1);
    obj -> SetTheta2(fTheta2);
    obj -> SetPairArrayIndex(fPairArrayIndex);
    obj -> SetPairChannel(fPairChannel);
    return obj;
}

void LKSiChannel::Print(Option_t *option) const
{
    if (TString(option).Index("!title")<0)
        e_info << "[LKSiChannel]" << std::endl;
    GETChannel::Print("!title");
    GETParameters::PrintParameters("!title");
    e_info << "- Det(" << fPadID << ") " << ((fSide==1)?"Junction":"Ohmic") << "(" << fStrip << "/" << fDirection << ")" << endl;
    e_info << "- Phi=(" << fPhi1 << "," << fPhi2 << "), Theta=(" << fTheta1 << "," << fTheta2 << ")" << endl;
}

TH1D *LKSiChannel::GetHist(TString name)
{
    if (name.IsNull())
        name = Form("hist_GET%d",GetCAAC());
    auto hist = fBufferRawSig.GetHist(name);
    hist -> SetTitle(MakeTitle());
    return hist;
}

bool LKSiChannel::IsPair(LKSiChannel* channel)
{
    int id1 = fPadID*1000+fSide*100+fStrip;
    int id2 = channel->GetDetID()*1000+channel->GetSide()*100+channel->GetStrip();
    if (id1==id2)
        return true;
    return false;
}
