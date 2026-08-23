#ifndef STDPulseAnalyzer_HH
#define STDPulseAnalyzer_HH
#include "LKTask.h"
#include "LKRun.h"
#include "LKParameterContainer.h"
#include "GETChannel.h"

#include "TProfile.h"
#include "TSystem.h"
#include "TSpectrum.h"

class STDPulseAnalyzer : public LKTask
{ 
    public:
        enum PulseMethod
        {
            kMaximum = 0,
            kFullFit = 1
        };

        struct ChannelHit
        {
            int HitNum;
            vector<double> ADC;
            vector<double> TB;

            void Clear(){
                ADC.clear();
                TB.clear();
                HitNum = 0;
            };

            void AddHit(double adc, double tb){
                ADC.push_back(adc);
                TB.push_back(tb);
                HitNum++;
            };

            double GetADC(int idx=0){
                if(idx >= HitNum || idx < 0){return -999.;}
                return ADC[idx];
            };
            double GetTB(int idx=0){
                if(idx >= HitNum || idx < 0){return -999.;}
                return TB[idx];
            };

        }ChannelHit;

    public:
        STDPulseAnalyzer();
        virtual ~STDPulseAnalyzer() {}

        bool Init();

        void SetNextChannel(GETChannel* channel);
        struct ChannelHit GetChannelHit();

        double GetPulseTemplate(int tb);
        int GetPulseTemplateMaxTB(){return fPulseTemplateArrNum;}

    private:
        void InitAnalyzer();
        void InitPulseTemplate();

        void ClearChannel();
        void FindSaturatedRange();

        void PulseMaximumAnalyzer(); // Method 0
        void PulseFullFitAnalyzer(); // Method 1

        void FillPulseShape();
        void MakePulseShape();

        Double_t SinglePulseFitter(Double_t* x, Double_t* par);
        Double_t MultiPulseFitter(Double_t* x, Double_t* par);

        LKRun* fRun; 
        LKParameterContainer* fPar;

        int fMethodOpt; // [0 == Maximum Point, to be updated]

        double fThreshold;
        int fTBStartIdx = 30;
        int fTBEndIdx = 480;

        GETChannel* fChannel;
        struct ChannelHit fChannelHit;

        // Pulse analyzers
        TSpectrum* fSpectrum;
        TH1D* fWaveForm;
        TF1* fPulseFitter;

        const double fSpectrumThreshold = 0.008; 
        const double fSpectrumSigma = 1.;

        bool fIsPositive;
        double fPedestal;
        vector<pair<double, double> > fSatuatedRange;

        // Pulse shape template 
        bool fMakePulseShapeMode;
        TString fPulseTemplatePath;
        TH2D* fPulseSum = nullptr;

        double fMaxADCBase = 100.;
        const double fMaxADC = 4070;
        static const int fPulseTemplateArrNum = 300;
        double fPulseTemplate[fPulseTemplateArrNum];
};

#endif
