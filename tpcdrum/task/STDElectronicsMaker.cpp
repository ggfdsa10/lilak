#include "STDElectronicsMaker.h"

ClassImp(STDElectronicsMaker);

STDElectronicsMaker::STDElectronicsMaker()
{
    fName = "STDElectronicsMaker";
}

bool STDElectronicsMaker::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();

    fChannelArray = fRun -> GetBranchA("RawPad");
    fMCTagTPCArray = fRun -> GetBranchA("MCTagTPCDrum");
    fMCTagSiArray = fRun -> GetBranchA("MCTagSiArray");
    fTrackArray = fRun -> GetBranchA("MCTrack");

    fDynamicRange = 240.; // [fC]
    if(fPar -> CheckPar("TPCDrum/DynamicRange")){
        fDynamicRange = fPar -> GetParDouble("TPCDrum/DynamicRange");
    }
    fEChargeToADC = fElectronCharge/(fDynamicRange *1.0e-15)*fADCMaxAmp;

    fTuneManager = STDSimTuningManager::GetSimTuningManager();
    fPulseAnalyzer = new STDPulseAnalyzer();
    fPulseAnalyzer -> Init();

    return true;
}

void STDElectronicsMaker::Exec(Option_t *option)
{
    fTuneManager -> InitNoise(); // Initialize the event noise

    const int trackNum = fTrackArray -> GetEntries();
    const int chanNum = fChannelArray -> GetEntries();
    int tpcPadNum = fPadPlane -> GetPadNum();
    double trackWeights[trackNum][512];

    for(int chan=0; chan<chanNum; chan++){
        fChannel = (GETChannel*)fChannelArray -> At(chan);

        bool isTPC = (chan <= tpcPadNum) ? false : true;
        if(isTPC){fMCTag = (LKMCTag*)fMCTagTPCArray -> At(chan);}
        else{fMCTag = (LKMCTag*)fMCTagSiArray -> At(chan);}

        memset(trackWeights, 0., sizeof(trackWeights));

        auto ADC = fChannel -> GetWaveformY();

        // Step1: Seperate the mc raw weight by tracks
        for(int tb=0; tb<512; tb++){
            int mcNum = fMCTag -> GetMCNum(tb);

            for(int id=0; id<mcNum; id++){
                int mcTrkIDIdx = fMCTag -> GetMCID(id, tb) -1;
                double purity = fMCTag -> GetMCPurity(id, tb);
                if(isTPC){trackWeights[mcTrkIDIdx][tb] = purity * ADC[tb] * fEChargeToADC;}
                else{trackWeights[mcTrkIDIdx][tb] = purity * ADC[tb];}
            }
        }

        // Initializa the saving data
        fMCTag -> Clear();
        memset(ADC, 0., sizeof(int)*512);
        double trkADC[512];

        // Step2: Make the pulse shape by tracks
        for(int trk=0; trk<trackNum; trk++){
            memset(trkADC, 0., sizeof(trkADC));

            // Generate the small pulse with weights in each time bucket
            // Final pulse will be accmulated in ADC array
            for(int tb=0; tb<512; tb++){
                double w = trackWeights[trk][tb];

                // Small pulse generation loop
                for(int tb2=0; tb2<300; tb2++){ 
                    int baseTB = tb2-100 + tb;
                    if(baseTB < 0 || baseTB >= 512){continue;}
                    double pulse = w * fPulseAnalyzer->GetPulseTemplate(tb2);
                    ADC[baseTB] += pulse;
                    trkADC[baseTB] += pulse;
                }
            }
            // Step3: Save the Truth pulse data into MCTag
            for(int tb=0; tb<512; tb++){
                fMCTag -> AddMCWeightTag(trk+1, trkADC[tb], tb);
            }
        }

        // Step4: If channel is for Si, make the ohmic and junction adc pulse
        if(!isTPC){
            int agetID = fChannel -> GetAget();
            int chanID = fChannel -> GetChan();
            int ohmicID = fSiArray -> GetOhmicID(agetID, chanID);
            // make the negetive charge for junction channe;
            if(ohmicID == -1){ 
                for(int tb=0; tb<512; tb++){
                    ADC[tb] = -ADC[tb];
                    ADC[tb] += fADCMaxAmp;
                }
            }
        }

        // Step5: Make the Noise
        int asadID = fChannel -> GetAsad();
        fTuneManager -> AddChannelNoise(asadID, ADC);
    }
}

bool STDElectronicsMaker::EndOfRun()
{
    return true;
}
