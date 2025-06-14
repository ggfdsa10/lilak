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
    fSide = 0;
    fStrip = -1;
    fDirection = -1;
    fInverted = false;
    fPairArrayIndex = -1;
    fPairChannel = nullptr; //!
    fPhi1 = -99.9;
    fPhi2 = -99.9;
    fTheta1 = -99.9;
    fTheta2 = -99.9;
    fEnergy = -99.9;
    fEnergy2 = -99.9;

}

void LKSiChannel::Copy(TObject &object) const
{
    GETChannel::Copy(object);
    ((LKSiChannel&)object).SetSide(fSide);
    ((LKSiChannel&)object).SetStrip(fStrip);
    ((LKSiChannel&)object).SetDirection(fDirection);
    ((LKSiChannel&)object).SetInverted(fInverted);
    ((LKSiChannel&)object).SetPhi1(fPhi1);
    ((LKSiChannel&)object).SetPhi2(fPhi2);
    ((LKSiChannel&)object).SetTheta1(fTheta1);
    ((LKSiChannel&)object).SetTheta2(fTheta2);
}

TObject* LKSiChannel::Clone(const char *newname) const
{
    LKSiChannel *obj = (LKSiChannel*) GETChannel::Clone(newname);
    obj -> SetLocalID(fLocalID);
    obj -> SetSide(fSide);
    obj -> SetStrip(fStrip);
    obj -> SetInverted(fInverted);
    obj -> SetPhi1(fPhi1);
    obj -> SetPhi2(fPhi2);
    obj -> SetTheta1(fTheta1);
    obj -> SetTheta2(fTheta2);
    obj -> SetPairArrayIndex(fPairArrayIndex);
    obj -> SetPairChannel(fPairChannel);
    return obj;
}

const char* LKSiChannel::GetName() const
{
    return Form("Ch%d_%d_Det%d_%d_%s_%d_%s", fChannelID,fLocalID,fDetType,fPadID,(fSide==0?"Junction":"Ohmic"),fStrip,(fSide==0?(fDirection==0?"L":"R"):(fDirection==0?"U":"D")));
}

const char* LKSiChannel::GetTitle() const
{
    return Form("Channel%d(%d) : Det%d(%d) %s(%d)%s CAAC(%d,%d,%d,%d)", fChannelID,fLocalID,fDetType,fPadID,(fSide==0?"Junction":"Ohmic"),fStrip,(fSide==0?(fDirection==0?"L":"R"):(fDirection==0?"U":"D")),fCobo,fAsad,fAget,fChan);
}

void LKSiChannel::Print(Option_t *option) const
{
    if (TString(option).Contains("get")) {
        e_info << "- CAAC = " << fCobo << " " << fAsad << " " << fAget << " " << fChan << " | DSSD = " << fPadID << " " << ((fSide==0)?"Junc.":"Ohmic") << " " << fStrip << " " << fDirection << " | E = " << fEnergy << endl;
        return;
    }
    if (TString(option).Index("!title")<0)
        e_info << "[LKSiChannel]" << std::endl;
    GETChannel::Print("!title");
    e_info << "- Det(" << fPadID << ") " << "gCh|lCh=(" << fChannelID << "|" << fLocalID << ") " << ((fSide==0)?"Junction":"Ohmic") << "(" << fStrip << "/" << fDirection << ")" << endl;
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
    if (fPadID==channel->GetDetID() && fSide==channel->GetSide() && fStrip==channel->GetStrip() && fDirection!=channel->GetDirection())
        return true;
    return false;
}

double LKSiChannel::GetIntegral(double pedestal, bool inverted)
{
    if (pedestal<0) pedestal = fPedestal;
    if (pedestal<0) pedestal = 0;
    inverted = fInverted;
    return fBufferRawSig.Integral(pedestal, inverted);
}
