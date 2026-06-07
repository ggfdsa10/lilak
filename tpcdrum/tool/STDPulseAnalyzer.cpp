#include "STDPulseAnalyzer.h"

STDPulseAnalyzer::STDPulseAnalyzer()
: fMethodOpt(kFullFit), fMakePulseShapeMode(false), fThreshold(40)
{
}

bool STDPulseAnalyzer::Init()
{
    fRun = LKRun::GetRun();
    fPar = fRun -> GetParameterContainer();

    if(fPar->CheckPar("STDPulseAnalyzer/Method")){
        fMethodOpt = fPar->GetParInt("STDPulseAnalyzer/Method");
    }
    if(fPar->CheckPar("STDPulseAnalyzer/ADCThreshold")){
        fThreshold = fPar->GetParDouble("STDPulseAnalyzer/ADCThreshold");
    }
    if(fPar->CheckPar("STDPulseAnalyzer/OnMakePulseShape")){
        fMakePulseShapeMode = fPar->GetParBool("STDPulseAnalyzer/OnMakePulseShape");
    }

    InitPulseTemplate();

    if(fMakePulseShapeMode){
        fPulseSum = new TH2D("pulseSum", "", 300, 0, 300, 300, 0, 1.);
        fPulseSum -> SetStats(0);
        fPulseSum -> SetTitle("Summed pulse; TB; ADC fraction");
    }

    return true;
}

void STDPulseAnalyzer::SetNextChannel(GETChannel* channel)
{
    ClearChannel();
    fChannel = channel;
    fWaveForm = fChannel -> GetHist();
    fPedestal = fChannel -> GetPedestal();

    FindSaturatedRange();

    if(fMethodOpt==kMaximum){PulseMaximumAnalyzer();}
    else if(fMethodOpt==kFullFit){PulseFullFitAnalyzer();}

    if(fMakePulseShapeMode){FillPulseShape();}
}

struct STDPulseAnalyzer::ChannelHit STDPulseAnalyzer::GetChannelHit(){return fChannelHit;}

double STDPulseAnalyzer::GetPulseTemplate(int tb)
{
    if(tb < 0 || tb >= 300){return 0.;}
    return fPulseTemplate[tb];
}

void STDPulseAnalyzer::InitAnalyzer()
{
    fSpectrum = new TSpectrum();
}

void STDPulseAnalyzer::InitPulseTemplate()
{
    TString dataPath = "";
    if(fPar->CheckPar("TPCDrum/SimDataPath")){
        dataPath = fPar->GetParString("TPCDrum/SimDataPath");
        if(dataPath[dataPath.Sizeof()-1] != '/'){dataPath += "/";}
    }

    double peakingTime = 502;
    double tbUnit = 40;
    if(fPar -> CheckPar("STDPulseAnalyzer/PeakingTime")){
        peakingTime = fPar -> GetParDouble("STDPulseAnalyzer/PeakingTime");
    }
    if(fPar -> CheckPar("TPCDrum/TimeBucketUnit")){
        tbUnit = fPar -> GetParDouble("TPCDrum/TimeBucketUnit");
    }
    if(fPar -> CheckPar("TPCDrum/PulseShapeData")){
        dataPath += fPar -> GetParString("TPCDrum/PulseShapeData");
    }

    TFile* pulseFile = new TFile(dataPath, "read");
    if(!pulseFile->IsOpen()){
        cout << "There is no " << dataPath << ", Turn on the Pulse shape analysis mode." << endl;
        fMakePulseShapeMode = true;
        return;
    }
    cout << "Found a pulse data, " << dataPath << endl;

    TTree* pulseTree = (TTree*)pulseFile -> Get("pulse");    
    pulseTree -> SetBranchAddress("template", &fPulseTemplate);
    pulseTree -> GetEntry(0);
}

void STDPulseAnalyzer::ClearChannel()
{
    fChannelHit.Clear();
    fSatuatedRange.clear();
    fIsPositive = true;
    fPedestal = 0.;
}

void STDPulseAnalyzer::FindSaturatedRange()
{
    int prevTB = -1;
    int lastTB = -1;
    for(int tb=1; tb<510; tb++){
        double preADC = fWaveForm -> GetBinContent(tb) + fPedestal;
        double currADC = fWaveForm -> GetBinContent(tb+1) + fPedestal;
        double nextADC = fWaveForm -> GetBinContent(tb+2) + fPedestal;
        if(currADC >= fMaxADC){
            if(preADC < fMaxADC && nextADC >= fMaxADC){prevTB = tb+1;}
            if(preADC >= fMaxADC && nextADC < fMaxADC){lastTB = tb;}
            if(prevTB != -1 && lastTB != -1){
                fSatuatedRange.push_back(make_pair(prevTB, lastTB));
                prevTB = -1;
                lastTB = -1;
            }
        }
    }
}

void STDPulseAnalyzer::PulseMaximumAnalyzer()
{
    double maxADC = fWaveForm -> GetMaximum();
    double maxTB = fWaveForm -> GetMaximumBin();

    int saturationNum = fSatuatedRange.size();
    bool isSaturation = false;
    for(int i=0; i<saturationNum; i++){
        double prevTB = fSatuatedRange[i].first - 1;
        double lastTB = fSatuatedRange[i].second;
        if(prevTB <= maxTB && maxTB <= lastTB){isSaturation = true;}
    }
    if(isSaturation){return;}
    if(maxTB < fTBStartIdx || maxTB > fTBEndIdx){return;}
    if(maxADC > fThreshold){fChannelHit.AddHit(maxADC, maxTB);}
}

void STDPulseAnalyzer::PulseFullFitAnalyzer()
{
    // Step1: Search the pulse peaks
    fSpectrum -> Search(fWaveForm, fSpectrumSigma, "nodraw goff", fSpectrumThreshold);
    const int numPeak = fSpectrum->GetNPeaks();
    auto peakX = fSpectrum->GetPositionX();
    auto peakY = fSpectrum->GetPositionY();
    int satuIdx[numPeak];
    fill_n(&satuIdx[0], numPeak, -1);

    // Step2: Find the real number of pulse peak
    const double saturationTBDist = 2.;
    int peakNum = 0;
    for(int p=0; p<numPeak; p++){
        double tb = peakX[p];
        double adc = peakY[p];
        if(adc < fThreshold){continue;}

        // Find the closed saturation range if there is exist.
        int saturationNum = fSatuatedRange.size();
        for(int s=0; s<saturationNum; s++){
            double prevTB = fSatuatedRange[s].first - 1;
            double lastTB = fSatuatedRange[s].second;
            if(prevTB <= tb && tb <= lastTB){
                satuIdx[p] = s;
                break;
            }
        }

        // Reject the saturated fake peak point
        double prevDistTB = fabs(tb-fSatuatedRange[satuIdx[p]].first);
        double lastDistTB = fabs(tb-fSatuatedRange[satuIdx[p]].second);
        if(lastDistTB < saturationTBDist){
            peakX[p] = -99999.;
            peakY[p] = -99999.;
            continue;
        }
        if(prevDistTB < saturationTBDist){
            peakX[p] = fSatuatedRange[satuIdx[p]].first + (fSatuatedRange[satuIdx[p]].second - fSatuatedRange[satuIdx[p]].first)/2.;
            peakY[p] = fMaxADC+fMaxADCBase*5;
        }
        peakNum++;
    }

    // Step3: Fit the full channel using multi pulse fit
    fPulseFitter = new TF1(Form("PulseFullFit"), this, &STDPulseAnalyzer::MultiPulseFitter, fTBStartIdx, fTBEndIdx, 3*peakNum+2);
    fPulseFitter -> FixParameter(0, peakNum);
    fPulseFitter -> FixParameter(1, 0.);

    int PulseIdx = 0;
    for(int p=0; p<numPeak; p++){
        double tb = peakX[p];
        double adc = peakY[p];
        if(adc < fThreshold){continue;}
        
        fPulseFitter -> SetParameter(2+PulseIdx*3, tb);
        fPulseFitter -> SetParameter(2+PulseIdx*3+1, adc);
        fPulseFitter -> SetParameter(2+PulseIdx*3+2, satuIdx[p]);
        if(adc >= fMaxADC){
            fPulseFitter -> SetParLimits(2+PulseIdx*3, fSatuatedRange[satuIdx[p]].first, fSatuatedRange[satuIdx[p]].second);
            fPulseFitter -> SetParLimits(2+PulseIdx*3+1, fMaxADC, 10000.);
            fPulseFitter -> SetParLimits(2+PulseIdx*3+2, satuIdx[p], satuIdx[p]);
        }
        PulseIdx++;
    }
    fWaveForm -> Fit(fPulseFitter, "RQIM");

    // Step4: Save the hits
    for(int p=0; p<PulseIdx; p++){
        double recoTB = fPulseFitter->GetParameter(2+p*3);
        double recoADC = fPulseFitter->GetParameter(2+p*3+1);
        fChannelHit.AddHit(recoADC, recoTB);
    }
}

void STDPulseAnalyzer::FillPulseShape()
{
    if(fChannelHit.HitNum != 1){return;}

    double maxADC = fChannelHit.GetADC();
    double maxTB = fChannelHit.GetTB();
    if(maxADC < fThreshold || maxADC > 3450){return;}

    double normFactorADC = 1./maxADC;
    double shiftTB = fMaxADCBase -maxTB -0.5;

    for(int tb=fTBStartIdx; tb<fTBEndIdx; tb++){
        double rawADC = fWaveForm -> GetBinContent(tb+1);
        double adc = rawADC * normFactorADC;
        if(tb == fMaxADCBase || adc < 0.001){continue;}
        fPulseSum -> Fill(tb+shiftTB, adc);
    }
}

void STDPulseAnalyzer::MakePulseShape()
{
    double peakingTime = fPar -> GetParDouble("TPCDrum/PeakingTime");
    double tbUnit = fPar -> GetParDouble("TPCDrum/TBUnit");

    double tbBoundaryLower = fMaxADCBase - peakingTime/tbUnit*2. -10;
    double tbBoundaryUpper = fMaxADCBase + peakingTime/tbUnit*4. -10;
    
    TH1D* pulseProfile = new TH1D("pulseProfile", "Pulse_Profile", 300, 0., 300.);

    double offsetADC = 0.;
    double offsetNum = 0.;
    for(int bx=0; bx<fPulseSum->GetNbinsX(); bx++){
        double xValue = fPulseSum -> GetXaxis() -> GetBinCenter(bx+1);

        double mean = 0.;
        double weight = 0.;
        for(int by=0; by<fPulseSum->GetNbinsY(); by++){
            double entry = fPulseSum -> GetBinContent(bx+1, by+1);
            double yValue = fPulseSum -> GetYaxis() -> GetBinCenter(by+1);
            mean += (pow(entry,4.)*yValue);
            weight += pow(entry,4.);
                   
        }
        mean /= weight;
        pulseProfile -> Fill(xValue, mean);
        
        if(xValue < tbBoundaryLower){
            offsetADC += mean;
            offsetNum += 1.;
        }
    }
    // subtract the remnant offset
    offsetADC /= offsetNum;
    for(int bx=0; bx<pulseProfile->GetNbinsX(); bx++){
        double xValue = pulseProfile -> GetXaxis() -> GetBinCenter(bx+1);
        double entry = pulseProfile -> GetBinContent(bx+1);
        fPulseTemplate[bx] = entry - offsetADC;
    }
    for(int tb=0; tb<fPulseTemplateArrNum; tb++){
        if(tb < tbBoundaryLower){fPulseTemplate[tb] = 0.;} // calibrate pulse before starting of pulse
        if(tb > tbBoundaryLower){fPulseTemplate[tb] = 0.;} // calibrate pulse before starting of pulse
    }

    // Save the pulse template
    TFile* pulseFile = new TFile(Form("%sPulse_template_%iPT_%ins.root", fPulseTemplatePath.Data(), int(peakingTime), int(tbUnit)), "recreate");
    TTree* pulseTree = new TTree("pulse", "pulse");
    pulseTree -> Branch("template", fPulseTemplate, Form("template[%i]/D", fPulseTemplateArrNum));
    pulseTree -> Branch("PeakingTime", &peakingTime, "peakingTime/D");
    pulseTree -> Branch("TbUnit", &tbUnit, "TbUnit/D");
    pulseTree -> Fill();
    pulseFile -> cd();
    pulseTree -> Write();
    pulseFile -> Close();

    // drawing pulse template
    TCanvas* cPulseCanvas = new TCanvas("","",1200,1200);
    cPulseCanvas -> Divide(2,1);
    cPulseCanvas -> cd(1);
    gPad -> SetLogz();
    fPulseSum -> Draw("colz");
    pulseProfile -> SetLineColor(kRed);
    pulseProfile -> SetLineWidth(2);
    pulseProfile -> Draw("same, hist");
    cPulseCanvas -> Draw();

    cPulseCanvas -> SaveAs(Form("./pulse_template_%iPT_%ins.pdf", int(peakingTime), int(tbUnit)));
}

Double_t STDPulseAnalyzer::SinglePulseFitter(Double_t* x, Double_t* par)
{
    int tbIdx = x[0] - (par[0] - fMaxADCBase);
    double adc = fabs(par[1]);
    if(tbIdx < 0){return par[2]+adc*fPulseTemplate[0];}
    else if(tbIdx >= fPulseTemplateArrNum){return par[2]+adc*fPulseTemplate[fPulseTemplateArrNum-1];}
    return par[2]+adc*fPulseTemplate[tbIdx];
}

Double_t STDPulseAnalyzer::MultiPulseFitter(Double_t* x, Double_t* par)
{
    int pulseNum = par[0];
    Double_t pedestal = par[1];

    Double_t sumFit = 0.;
    for(int i=0; i<pulseNum; i++){
        int parIdx = 2+i*3;

        int satuIdx = par[parIdx+2];
        double prevSatuRange = fSatuatedRange[satuIdx].first;
        double lastSatuRange = fSatuatedRange[satuIdx].second;
        if(prevSatuRange < x[0] && x[0] < lastSatuRange){TF1::RejectPoint(); return 0;}
        if(lastSatuRange > 1. && x[0] > lastSatuRange+512.){TF1::RejectPoint(); return 0;}

        int tbIdx = x[0] - (par[parIdx] - fMaxADCBase);
        double adc = par[parIdx+1];

        if(tbIdx < 0 || tbIdx >= fPulseTemplateArrNum){continue;}
        sumFit += adc*fPulseTemplate[tbIdx];
    }
    return sumFit+pedestal;
}