#include "LKHTTrackingTask.h"
#include "LKGeoBox.h"
#include <vector>

ClassImp(LKHTTrackingTask);

LKHTTrackingTask::LKHTTrackingTask()
{
    fName = "LKHTTrackingTask";
}

bool LKHTTrackingTask::Init()
{
    lk_info << "Initializing LKHTTrackingTask" << std::endl;

    fPar -> UpdateBinning("LKHTTrackingTask/x_binning  #100 -100 100", fNX, fX1, fX2);
    fPar -> UpdateBinning("LKHTTrackingTask/y_binning  #100 -100 100", fNY, fY1, fY2);
    fPar -> UpdateBinning("LKHTTrackingTask/z_binning  #100 -100 100", fNZ, fZ1, fZ2);
    fPar -> UpdateBinning("LKHTTrackingTask/r_binning  #100  0 0  # 0 0 will set range automatically", fNR, fR1, fR2);
    fPar -> UpdateBinning("LKHTTrackingTask/t_binning  #100  0 0  # 0 0 will set range automatically", fNT, fT1, fT2);
    if (fPar -> CheckPar("LKHTTrackingTask/transform_center  0 0 0"))
        fPar -> UpdateV3("LKHTTrackingTask/transform_center  0 0 0", fTCX, fTCY, fTCZ);
    else {
        fTCX = fX1;
        fTCY = fY1;
        fTCZ = fZ1;
    }
    fPar -> UpdatePar(fNumHitsCut,"LKHTTrackingTask/num_hits_cut  3");

    auto fTrackTemp = new LKLinearTrack();

    fTracker0 = new LKHTLineTracker();
    fTracker1 = new LKHTLineTracker();

    auto SetTracker = [](LKHTLineTracker* tk, double tx, double ty, int nx, double x1, double x2, int ny, double y1, double y2, int nr, double r1, double r2, int nt, double t1, double t2) {
        tk -> SetTransformCenter(tx, ty);
        tk -> SetImageSpaceRange(nx, x1, x2, ny, y1, y2);
        if (r1==0&&r2==0&&t1==0&&t2==0)
            tk -> SetParamSpaceBins(nr, nt);
        else
            tk -> SetParamSpaceRange(nr, r1, r2, nt, t1, t2);
        tk -> SetCorrelateBoxBand();
        tk -> SetWFConst();
        tk -> Print();
    };

    fNumHT = fPar -> GetParN("LKHTTrackingTask/mode");
    if (fNumHT==1) {
        fAxisConf0 = fPar -> GetParString("LKHTTrackingTask/mode");
        fAxisConf0.ToLower();
        if      (fAxisConf0=="xy") { fAxis00 = LKVector3::kX; fAxis01 = LKVector3::kY; SetTracker(fTracker0, fTCX,fTCY, fNX,fX1,fX2, fNY,fY1,fY2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="xz") { fAxis00 = LKVector3::kX; fAxis01 = LKVector3::kZ; SetTracker(fTracker0, fTCX,fTCZ, fNX,fX1,fX2, fNZ,fZ1,fZ2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="yz") { fAxis00 = LKVector3::kY; fAxis01 = LKVector3::kZ; SetTracker(fTracker0, fTCY,fTCZ, fNY,fY1,fY2, fNZ,fZ1,fZ2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="yx") { fAxis00 = LKVector3::kY; fAxis01 = LKVector3::kX; SetTracker(fTracker0, fTCY,fTCX, fNY,fY1,fY2, fNX,fX1,fX2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="zx") { fAxis00 = LKVector3::kZ; fAxis01 = LKVector3::kX; SetTracker(fTracker0, fTCZ,fTCX, fNZ,fZ1,fZ2, fNX,fX1,fX2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="zy") { fAxis00 = LKVector3::kZ; fAxis01 = LKVector3::kY; SetTracker(fTracker0, fTCZ,fTCY, fNZ,fZ1,fZ2, fNY,fY1,fY2, fNR,fR1,fR2, fNT,fT1,fT2); }
        lk_info << "Axis: " << fAxis00 << ", " << fAxis01 << endl;
    }
    else if (fNumHT==2) {
        fAxisConf0 = fPar -> GetParString("LKHTTrackingTask/mode",0);
        fAxisConf0.ToLower();
        if      (fAxisConf0=="xy") { fAxis00 = LKVector3::kX; fAxis01 = LKVector3::kY; SetTracker(fTracker0, fTCX,fTCY, fNX,fX1,fX2, fNY,fY1,fY2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="xz") { fAxis00 = LKVector3::kX; fAxis01 = LKVector3::kZ; SetTracker(fTracker0, fTCX,fTCZ, fNX,fX1,fX2, fNZ,fZ1,fZ2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="yz") { fAxis00 = LKVector3::kY; fAxis01 = LKVector3::kZ; SetTracker(fTracker0, fTCY,fTCZ, fNY,fY1,fY2, fNZ,fZ1,fZ2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="yx") { fAxis00 = LKVector3::kY; fAxis01 = LKVector3::kX; SetTracker(fTracker0, fTCY,fTCX, fNY,fY1,fY2, fNX,fX1,fX2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="zx") { fAxis00 = LKVector3::kZ; fAxis01 = LKVector3::kX; SetTracker(fTracker0, fTCZ,fTCX, fNZ,fZ1,fZ2, fNX,fX1,fX2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf0=="zy") { fAxis00 = LKVector3::kZ; fAxis01 = LKVector3::kY; SetTracker(fTracker0, fTCZ,fTCY, fNZ,fZ1,fZ2, fNY,fY1,fY2, fNR,fR1,fR2, fNT,fT1,fT2); }
        fAxisConf1 = fPar -> GetParString("LKHTTrackingTask/mode",1);
        fAxisConf1.ToLower();
        if      (fAxisConf1=="xy") { fAxis10 = LKVector3::kX; fAxis11 = LKVector3::kY; SetTracker(fTracker1, fTCX,fTCY, fNX,fX1,fX2, fNY,fY1,fY2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf1=="xz") { fAxis10 = LKVector3::kX; fAxis11 = LKVector3::kZ; SetTracker(fTracker1, fTCX,fTCZ, fNX,fX1,fX2, fNZ,fZ1,fZ2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf1=="yz") { fAxis10 = LKVector3::kY; fAxis11 = LKVector3::kZ; SetTracker(fTracker1, fTCY,fTCZ, fNY,fY1,fY2, fNZ,fZ1,fZ2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf1=="yx") { fAxis10 = LKVector3::kY; fAxis11 = LKVector3::kX; SetTracker(fTracker1, fTCY,fTCX, fNY,fY1,fY2, fNX,fX1,fX2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf1=="zx") { fAxis10 = LKVector3::kZ; fAxis11 = LKVector3::kX; SetTracker(fTracker1, fTCZ,fTCX, fNZ,fZ1,fZ2, fNX,fX1,fX2, fNR,fR1,fR2, fNT,fT1,fT2); }
        else if (fAxisConf1=="zy") { fAxis10 = LKVector3::kZ; fAxis11 = LKVector3::kY; SetTracker(fTracker1, fTCZ,fTCY, fNZ,fZ1,fZ2, fNY,fY1,fY2, fNR,fR1,fR2, fNT,fT1,fT2); }
        lk_info << "Axis-0: " << fAxis00 << ", " << fAxis01 << endl;
        lk_info << "Axis-1: " << fAxis10 << ", " << fAxis11 << endl;
    }
    else {
        lk_error << "Must set parameter LKHTTrackingTask/mode # xy" << endl;
        return false;
    }

    std::vector<TString> hitBranchNameArray = fPar -> GetParVString("LKHTTrackingTask/hit_branch_name  Hit");
    if (hitBranchNameArray.size()==0)
        hitBranchNameArray.push_back(TString("Hit"));
    for (auto hitBranchName : hitBranchNameArray) {
        lk_debug << "hit branch " << hitBranchName << endl;
        fHitArray[fCountHitBranches] = nullptr;
        fHitArray[fCountHitBranches] = fRun -> GetBranchA(hitBranchName);
        ++fCountHitBranches;
    }

    fTrackArray = fRun -> RegisterBranchA("Track","LKLinearTrack",100);

    return true;
}

void LKHTTrackingTask::Exec(Option_t *option)
{
    fTrackArray -> Clear("C");
    fNumTracks = 0;

    auto numHitsAll = 0;
    for (auto iHitArray=0; iHitArray<fCountHitBranches; ++iHitArray)
        numHitsAll += fHitArray[iHitArray] -> GetEntries();

    if (numHitsAll<fNumHitsCut) {
        lk_info << "Number of hits " << numHitsAll << " lower than cut " << fNumHitsCut << endl;
        return;
    }

    if (fNumHT==1) {
        // 1D -------------------------------------------------------------------------------------------
        fTracker0 -> Clear();

        for (auto iHitArray=0; iHitArray<fCountHitBranches; ++iHitArray) {
            auto hitArray = fHitArray[iHitArray];
            auto numHits = hitArray -> GetEntries();
            for (auto iHit=0; iHit<numHits; ++iHit)
            {
                auto hit = (LKHit*) hitArray -> At(iHit);
                fTracker0 -> AddHit(hit,fAxis00,fAxis01);
            }
        }

        fTracker0 -> Transform();
        auto paramPoint0 = fTracker0 -> FindNextMaximumParamPoint();
        fTrackTemp = fTracker0 -> FitTrack3DWithParamPoint(paramPoint0);
        if (fTrackTemp->GetQuality()>0)
        {
            auto track = (LKLinearTrack*) fTrackArray -> ConstructedAt(fNumTracks++);
            fTrackTemp -> Copy(*track);
            LKHit* hit;
            TIter next0(track->GetHitArray()); while ((hit = (LKHit*)next0())) track -> AddHit(hit);
        }
    }
    else {
        // 2D -------------------------------------------------------------------------------------------
        fTracker0 -> Clear();
        fTracker1 -> Clear();

        for (auto iHitArray=0; iHitArray<fCountHitBranches; ++iHitArray) {
            auto hitArray = fHitArray[iHitArray];
            auto numHits = hitArray -> GetEntries();
            for (auto iHit=0; iHit<numHits; ++iHit)
            {
                auto hit = (LKHit*) hitArray -> At(iHit);
                fTracker0 -> AddHit(hit,fAxis00,fAxis01);
                fTracker1 -> AddHit(hit,fAxis10,fAxis11);
            }
        }

        fTracker0 -> Transform();
        fTracker1 -> Transform();
        auto paramPoint0 = fTracker0 -> FindNextMaximumParamPoint();
        auto paramPoint1 = fTracker1 -> FindNextMaximumParamPoint();

        auto track0 = fTracker0 -> FitTrackWithParamPoint(paramPoint0);
        auto track1 = fTracker1 -> FitTrackWithParamPoint(paramPoint1);

        fTrackTemp -> Clear();
        fTrackTemp -> Create3DTrack(track0, fAxisConf0, track1, fAxisConf1);
        if (fTrackTemp->GetQuality()>0)
        {
            auto track = (LKLinearTrack*) fTrackArray -> ConstructedAt(fNumTracks++);
            fTrackTemp -> Copy(*track);
            LKHit* hit;
            TIter next0(track0->GetHitArray()); while ((hit = (LKHit*)next0())) track -> AddHit(hit);
            TIter next1(track1->GetHitArray()); while ((hit = (LKHit*)next1())) track -> AddHit(hit);
        }
    }

    for (auto iTrack=0; iTrack<fNumTracks; ++iTrack)
    {
        auto track = (LKLinearTrack*) fTrackArray -> At(iTrack);
        track -> SetTrackID(iTrack);
        LKGeoBox box(0.5*(fX2+fX1),0.5*(fY2+fY1),0.5*(fZ2+fZ1),fX2-fX1,fY2-fY1,fZ2-fZ1);
        TVector3 point1, point2;
        box.GetCrossingPoints(*track,point1,point2);
        track -> SetTrack(point1, point2);
    }

    lk_info << "Found " << fNumTracks << " tracks" << endl;
}
