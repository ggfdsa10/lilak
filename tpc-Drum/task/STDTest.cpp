#include "STDTest.h"

#include <vector>
#include <tuple>

ClassImp(STDTest);

STDTest::STDTest()
{
    fName = "STDTest";
}

bool STDTest::Init()
{
    fDetector = (TPCDrum *) fRun -> GetDetector();
    fPadPlane = (STDPadPlane*) fDetector -> GetDetectorPlane();
    fTrackArray = fRun -> KeepBranchA("MCTrack");
    fStepArray = fRun -> KeepBranchA("MCStepTPCDrum");

    for(int i=0; i<4; i++){
        hPlane[i] = new TH2D(Form("plane_%i", i), "", 50, -120., 120., 100, 0., 250.);
        hPlane[i] -> SetStats(0);

        hTrkEnergy[i][0] = new TH1D(Form("trkEnergyStart_%i", i), "", 100, 0., 150.);
        hTrkEnergy[i][1] = new TH1D(Form("trkEnergyEnd_%i", i), "", 100, 0., 150.);
        hTrkEnergy[i][0] -> SetStats(0);
        hTrkEnergy[i][1] -> SetStats(0);

        if(i == kNe20){
            hPlane[i] -> SetTitle("Ne20 position at y=251 mm plane; x [mm]; z [mm]");
            hTrkEnergy[i][0] -> SetTitle("Ne20 energy; Energy [MeV]; Counts");
            hTrkEnergy[i][1] -> SetTitle("Ne20 energy at y=251 mm; Energy [MeV]; Counts");
        }
        if(i == kC12){
            hPlane[i] -> SetTitle("C12 position at y=251 mm plane; x [mm]; z [mm]");
            hTrkEnergy[i][0] -> SetTitle("C12 energy; Energy [MeV]; Counts");
            hTrkEnergy[i][1] -> SetTitle("C12 energy at y=251 mm; Energy [MeV]; Counts");
        }
        if(i == kAlpha){
            hPlane[i] -> SetTitle("#alpha position aat y=251 mm plane; x [mm]; z [mm]");
            hTrkEnergy[i][0] -> SetTitle("#alpha energy; Energy [MeV]; Counts");
            hTrkEnergy[i][1] -> SetTitle("#alpha energy at y=251 mm; Energy [MeV]; Counts");
        }
    }

    hAlphaDistance = new TH1D("alphaDistance", "", 100, 0., 100.);
    hAlphaDistance -> SetStats(0);
    hAlphaDistance -> SetTitle("Distance B.T.W. #alpha; distance [mm]; Counts");

    hAlphaAngle = new TH1D("alphaDistance", "", 100, 0., 30.);
    hAlphaAngle -> SetStats(0);
    hAlphaAngle -> SetTitle("Angle B.T.W. #alpha; Opening angle [degree]; Counts");

    hAlphaAngleDistance = new TH2D("alphaDistance", "", 100, 0., 100., 100, 0., 30.);
    hAlphaAngleDistance -> SetStats(0);
    hAlphaAngleDistance -> SetTitle("; distance [mm]; Opening angle [degree]");

    return true;
}

void STDTest::Exec(Option_t *option)
{
    int trackNum = fTrackArray -> GetEntries();

    double trkEdep[20][3];
    memset(trkEdep, 0., sizeof(trkEdep));

    for(int trk=0; trk<trackNum; trk++){
        fMCTrack = (LKMCTrack*)fTrackArray -> UncheckedAt(trk);
        int pid = fMCTrack -> GetPDG();
        int pidIdx = -1;
        if(pid == id_ne20){pidIdx = kNe20;}
        if(pid == id_c12){pidIdx = kC12;}
        if(pid == id_c12_decay){pidIdx = kC12;}
        if(pid == id_c12_decay){pidIdx = kC12;}
        if(pid == id_alpha){pidIdx = kAlpha;}

        double x = fMCTrack->GetVX();
        double y = fMCTrack->GetVY();
        double z = fMCTrack->GetVZ();
        double e = fMCTrack->GetEnergy();
        fDetector -> GetCoordinateGeantToPad(x, y, z);

        if(pid == id_alpha){
            if(-40. <= y && y <= 40.){
            }
            else{
                return;
            }
        }
    }

    vector<pair<double, double>> alphaHitPos;
    vector<tuple<double, double, double>> alphaHitAngle;

    vector<int> tmpTrackId;
    int stepNum = fStepArray -> GetEntries();
    for(int i=0; i<stepNum; i++){
        fStep = (LKMCStep*)fStepArray -> UncheckedAt(i);

        double x = fStep -> GetX();
        double y = fStep -> GetY();
        double z = fStep -> GetZ();
        double t = fStep -> GetTime();
        double e = fStep -> GetEdep();

        fDetector -> GetCoordinateGeantToPad(x, y, z);

        int trackId = fStep -> GetTrackID(); // Note: Truth track Id for each step will be saved into LKMCTag format.

        fMCTrack  = (LKMCTrack*)fTrackArray -> UncheckedAt(trackId-1);
        double startE = fMCTrack->GetEnergy();
        int pid = fMCTrack -> GetPDG();
        int pidIdx = -1;
        if(pid == id_ne20){pidIdx = kNe20;}
        if(pid == id_c12){pidIdx = kC12;}
        if(pid == id_c12_decay){pidIdx = kC12;}
        if(pid == id_alpha){pidIdx = kAlpha;}

        if((siPlaneY/2. < fStep -> GetZ() && siPlaneY/2.+2. > fStep -> GetZ()) && (-120. <= x && x <= 120.) && (0. <= z && z <= 180.)){
            bool alreadyFill = false;
            for(int tmp=0; tmp<tmpTrackId.size(); tmp++){
                if(trackId == tmpTrackId[tmp]){alreadyFill = true;}
            }
            if(alreadyFill){continue;}

            hPlane[pidIdx] -> Fill(x, z);
            tmpTrackId.push_back(trackId);

            if(pidIdx == kAlpha){
                alphaHitPos.push_back(make_pair(x, z));

                double px = fMCTrack->GetPX();
                double py = fMCTrack->GetPY();
                double pz = fMCTrack->GetPZ();

                alphaHitAngle.push_back(make_tuple(px, py, pz));
            }
        }
        if((siPlaneY/2. > fStep -> GetZ())){
            trkEdep[trackId-1][0] = pidIdx;
            trkEdep[trackId-1][1] = startE;
            trkEdep[trackId-1][2] += e;
        }
    }

    for(int trk=0; trk<trackNum; trk++){
        bool findPassTrk = false;
        for(int i=0; i<tmpTrackId.size(); i++){
            int tmpTrkId = tmpTrackId[i] - 1;
            if(trk == tmpTrkId){
                findPassTrk = true;
            }
        }
        if(!findPassTrk){continue;}

        int pidIdx = trkEdep[trk][0]; 
        double startE = trkEdep[trk][1];
        if(startE < 0.1){continue;}
        double edep = trkEdep[trk][2];

        for(int i=0; i<4; i++){
            if(i == pidIdx){
                hTrkEnergy[i][0] -> Fill(startE);
                hTrkEnergy[i][1] -> Fill(startE - edep);
            }
        }
    }

    for(int i=0; i<alphaHitPos.size(); i++){
        double x1 = alphaHitPos[i].first;
        double z1 = alphaHitPos[i].second;

        double px1 = get<0>(alphaHitAngle[i]);
        double py1 = get<1>(alphaHitAngle[i]);
        double pz1 = get<2>(alphaHitAngle[i]);

        for(int j=i+1; j<alphaHitPos.size(); j++){
            double x2 = alphaHitPos[j].first;
            double z2 = alphaHitPos[j].second;

            double px2 = get<0>(alphaHitAngle[j]);
            double py2 = get<1>(alphaHitAngle[j]);
            double pz2 = get<2>(alphaHitAngle[j]);

            double x = x1-x2;
            double z = z1-z2;

            double distance = sqrt(x*x + z*z);
            hAlphaDistance -> Fill(distance);

            TVector3 alpha1(px1, py1, pz1);
            TVector3 alpha2(px2, py2, pz2);

            Double_t theta = alpha1.Angle(alpha2)*180/TMath::Pi();

            hAlphaAngle -> Fill(theta);

            hAlphaAngleDistance -> Fill(distance, theta);
        }
    }
}

bool STDTest::EndOfRun()
{



    TH2Poly* siPoly = new TH2Poly();
    for(int i=0; i<fDetector->fSiDetNum; i++){
        TString siName = fDetector->fSiDetectorName[i];

        double SiLocalPosX = fDetector->fSiDetectorCenter[i][0];
        double SiLocalPosZ = fDetector->fSiDetectorCenter[i][1];
        bool IsRotation = (i<6)? true : false;

        double siWidth = fDetector -> fSiWidth;
        double siHeight = fDetector -> fSiHeight;

        if(IsRotation){siHeight = fDetector -> fSiWidth;}
        if(IsRotation){siWidth = fDetector -> fSiHeight;}

        double x[5];
        double z[5];
        for(int i=0; i<5; i++){
            double xSign = (i<2 || i==4)? -1. : +1.;
            double zSign = (i==1 || i==2)? -1. : +1.;
            x[i] = SiLocalPosX + xSign*siWidth/2.;
            z[i] = SiLocalPosZ + zSign*siHeight/2.;
        }
        siPoly -> AddBin(5, x, z);
    }

    TCanvas* c1 = new TCanvas("","",1800, 2400);
    c1 -> Divide(3,4);

    for(int i=0; i<3; i++){
        const int cidx = i+1;
        c1 -> cd(cidx);
        hPlane[i] -> Draw("colz");
        siPoly -> Draw("same");
    }

    for(int i=0; i<3; i++){
        const int cidx = i+1+3;
        c1 -> cd(cidx);
        hTrkEnergy[i][0] -> Draw();
    }

    for(int i=0; i<3; i++){
        const int cidx = i+1+6;
        c1 -> cd(cidx);
        hTrkEnergy[i][1] -> Draw();
    }



    c1 -> cd(10);
    hAlphaDistance -> Draw();
    c1 -> cd(11);
    hAlphaAngle -> Draw();
    c1 -> cd(12);
    hAlphaAngleDistance -> Draw("colz");

    c1 -> Draw();
    c1 -> SaveAs(Form("mc_trkHit_siPlane%i.pdf", int(siPlaneY)));

    return true;
}