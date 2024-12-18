#include "LKDrawingGroup.h"
#include "LKDataViewer.h"
#include "LKMisc.h"
#include "TKey.h"
#include "TObjString.h"
#include "TSystem.h"

ClassImp(LKDrawingGroup)

LKDrawingGroup::LKDrawingGroup(TString name, int groupLevel)
: TObjArray()
{
    Init();
    if (name.EndsWith(".root"))
        if (AddFile(name))
            return;
    SetName(name);
}

LKDrawingGroup::LKDrawingGroup(TString fileName, TString groupSelection)
: TObjArray()
{
    Init();
    AddFile(fileName,groupSelection);
}

LKDrawingGroup::LKDrawingGroup(TFile* file, TString groupSelection)
: TObjArray()
{
    Init();
    AddFile(file,groupSelection);
}

void LKDrawingGroup::Init()
{
    fName = "drawings";
    fCvs = nullptr;
    fDivX = 0;
    fDivY = 0;
    fGroupLevel = 0;
}

LKDataViewer* LKDrawingGroup::CreateViewer()
{
    if (fViewer==nullptr)
        fViewer = new LKDataViewer(this);
    return fViewer;
}

void LKDrawingGroup::Draw(Option_t *option)
{
    TString optionString(option);
    optionString.ToLower();

    bool usingDataViewer = false;
    if (fViewer!=nullptr)
        usingDataViewer = true;
    if (usingDataViewer==false)
        usingDataViewer = (LKMisc::CheckOption(optionString,"v",true) || LKMisc::CheckOption(optionString,"viewer",true));

    if (usingDataViewer)
    {
        if (fViewer==nullptr) {
            LKDrawingGroup* group = this;
            if (IsDrawingGroup()) {
                lk_debug << endl;
                group = new LKDrawingGroup("top");
                group -> AddGroup(this);
            }
            fViewer = new LKDataViewer(group);
        }

        if (fViewer->IsActive())
            lk_warning << "viewer already running!" << endl;
        else
            fViewer -> Draw(optionString);
    }
    else
    {
        if (CheckIsGroupGroup() && TString(option)=="all")
        {
            auto numSub = GetEntries();
            for (auto iSub=0; iSub<numSub; ++iSub) {
                auto sub = (LKDrawingGroup*) At(iSub);
                sub -> SetGlobalOption(fGlobalOption);
                sub -> Draw(option);
            }
        }
        else
        {
            auto numDrawings = GetEntries();
            if (numDrawings>0)
            {
                ConfigureCanvas();
                if (numDrawings==1) {
                    auto drawing = (LKDrawing*) At(0);
                    drawing -> SetCanvas(fCvs);
                    drawing -> Draw(option);
                }
                else {
                    for (auto iDrawing=0; iDrawing<numDrawings; ++iDrawing)
                    {
                        auto drawing = (LKDrawing*) At(iDrawing);
                        drawing -> SetCanvas(fCvs->cd(iDrawing+1));
                        drawing -> Draw(option);
                    }
                }
            }
            //fCvs -> SetWindowSize(800,800);
        }
    }
}

void LKDrawingGroup::WriteFile(TString fileName)
{
    if (fileName.IsNull())
        fileName = Form("data_lilak/%s.root",fName.Data());
    e_info << "Writting " << fileName << endl;
    auto file = new TFile(fileName,"recreate");
    Write();
}

Int_t LKDrawingGroup::Write(const char *name, Int_t option, Int_t bsize) const
{
    int numWrite = 0;
    auto depth = GetGroupDepth();
    if (depth>=3) {
        auto numSub = GetNumGroups();
        e_info << "Writting " << numSub << " groups in " << fName << endl;
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            numWrite += sub -> Write("", option);
        }
        if (fPar!=nullptr) fPar -> Write();
        return numWrite;
    }
    else if (GetNumAllDrawingObjects()>1280)
    {
        auto numSub = GetNumGroups();
        e_info << "Writting " << numSub << " groups in " << fName << " (" << GetNumAllDrawingObjects() << ")" << endl;
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            numWrite += sub -> Write("", option);
        }
        if (fPar!=nullptr) fPar -> Write();
        return numWrite;
    }

    TString wName = name;
    if (wName.IsNull()) wName = fName;
    if (wName.IsNull()) wName = "top";
    TCollection::Write(wName, option, bsize);
    if (fPar!=nullptr) fPar -> Write();
    return 1;
}

void LKDrawingGroup::Save(bool recursive, bool saveRoot, bool saveImage, TString dirName, TString header, TString tag)
{
    if (dirName.IsNull()) {
        dirName = "data_lilak";
    }
    if (fName.IsNull()) {
        fName = "top";
    }
    gSystem -> Exec(Form("mkdir -p %s/",dirName.Data()));

    TString uheader = header;
    uheader.ReplaceAll(":","_");
    uheader.ReplaceAll(" ","_");
    if (uheader.IsNull()==false) uheader = uheader + "__";
    TString fullName = fName;//GetFullName();
    fullName.ReplaceAll(":","_");
    fullName.ReplaceAll(" ","_");
    if (!tag.IsNull()) tag = Form(".%s",tag.Data());

    if (fIsGroupGroup)
    {
        auto numSub = GetEntries();
        if (numSub<0) {
            lk_warning << "empty group" << endl;
            return;
        }

        if (GetGroupDepth()>=3) {
            dirName = dirName + "/" + fName;
            gSystem -> Exec(Form("mkdir -p %s/",dirName.Data()));
        }
        else {
            if (header.IsNull()) header = fName;
            else header = header + "_" + fName;
        }

        if (saveRoot) {
            TString fileName = dirName + "/" + uheader + fullName + tag + ".root";
            e_info << "Writting " << fileName << endl;
            auto file = new TFile(fileName,"recreate");
            Write();
        }

        if (saveImage) {
            auto sub0 = (LKDrawingGroup*) At(0);
            auto num0 = sub0 -> GetEntries();
            if (num0>4) {

            }
            for (auto iSub=0; iSub<numSub; ++iSub) {
                auto sub = (LKDrawingGroup*) At(iSub);
                sub -> Save(recursive, false, saveImage, dirName, header, tag);
            }
        }
    }
    else
    {
        if (saveRoot) {
            TString fileName = dirName + "/" + uheader + fullName + tag + ".root";
            e_info << "Writting " << fileName << endl;
            auto file = new TFile(fileName,"recreate");
            Write();
        }
        if (saveImage)
        {
            {
                TString dirNameImage = dirName + "/png";
                gSystem -> Exec(Form("mkdir -p %s/",dirNameImage.Data()));
                TString fileName = dirNameImage + "/" + uheader + fullName + tag + ".png";
                fCvs -> SaveAs(fileName);
            }
            {
                TString dirNameImage = dirName + "/pdf";
                gSystem -> Exec(Form("mkdir -p %s/",dirNameImage.Data()));
                TString fileName = dirNameImage + "/" + uheader + fullName + tag + ".pdf";
                fCvs -> SaveAs(fileName);
            }
            {
                TString dirNameImage = dirName + "/eps";
                gSystem -> Exec(Form("mkdir -p %s/",dirNameImage.Data()));
                TString fileName = dirNameImage + "/" + uheader + fullName + tag + ".eps";
                fCvs -> SaveAs(fileName);
            }
        }
    }
}

void LKDrawingGroup::Print(Option_t *opt) const
{
    TString option = opt;
    bool isTop = false;
    if (LKMisc::CheckOption(option,"level")==false) {
        isTop = true;
        e_info << "LKDrawingGroup " << fName << endl;
        option = option + ":level=0";
    }

    int tab = 0;
    if (LKMisc::CheckOption(option,"level"))
        tab = TString(LKMisc::FindOption(option,"level",false,1)).Atoi();
    TString header; for (auto i=0; i<tab; ++i) header += "  ";

    if (fIsGroupGroup)
    {
        auto numSub = GetEntries();
        TString title = header + Form("DrawingGroup[%d] %s",numSub,fName.Data());
        e_cout << title << endl;

        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            sub -> Print(option);
        }
    }
    else
    {
        auto numDrawings = GetEntries();
        e_cout << header << "Group " << fName << " containing " << numDrawings << " drawings" << endl;
        for (auto iDrawing=0; iDrawing<numDrawings; ++iDrawing)
        {
            auto drawing = (LKDrawing*) At(iDrawing);
            drawing -> Print(option);
        }
        if (LKMisc::CheckOption(option,"!drawing")==false) e_cout << endl;
    }
    if (isTop)
        e_info << GetNumAllDrawings() << " drawings in total" << endl;
}

bool LKDrawingGroup::ConfigureCanvas()
{
    auto numDrawings = GetEntries();

    if (fDivX==0 || fDivY==0)
    {
        if (CheckOption("wide_canvas"))
        {
            if      (numDrawings== 1) { fDivX =  1; fDivY =  1; }
            else if (numDrawings<= 2) { fDivX =  2; fDivY =  1; }
            else if (numDrawings<= 3) { fDivX =  3; fDivY =  1; }
            else if (numDrawings<= 6) { fDivX =  3; fDivY =  2; }
            else if (numDrawings<= 8) { fDivX =  4; fDivY =  2; }
            else if (numDrawings<= 9) { fDivX =  3; fDivY =  3; }
            else if (numDrawings<=12) { fDivX =  4; fDivY =  3; }
            else if (numDrawings<=16) { fDivX =  4; fDivY =  4; }
            else if (numDrawings<=20) { fDivX =  4; fDivY =  5; }
            else if (numDrawings<=24) { fDivX =  4; fDivY =  6; }
            else if (numDrawings<=28) { fDivX =  4; fDivY =  7; }
            else if (numDrawings<=32) { fDivX =  4; fDivY =  8; }
            else if (numDrawings<=36) { fDivX =  4; fDivY =  9; }
            else if (numDrawings<=40) { fDivX =  4; fDivY =  10; }
            else {
                lk_error << "Too many drawings!!! " << numDrawings << endl;
                return false;
            }
        }

        else if (CheckOption("vertical_canvas"))
        {
            if      (numDrawings== 1) { fDivY =  1; fDivX =  1; }
            else if (numDrawings<= 2) { fDivY =  2; fDivX =  1; }
            else if (numDrawings<= 4) { fDivY =  2; fDivX =  2; }
            else if (numDrawings<= 6) { fDivY =  3; fDivX =  2; }
            else if (numDrawings<= 8) { fDivY =  4; fDivX =  2; }
            else if (numDrawings<= 9) { fDivY =  3; fDivX =  3; }
            else if (numDrawings<=12) { fDivY =  4; fDivX =  3; }
            else if (numDrawings<=16) { fDivY =  4; fDivX =  4; }
            else if (numDrawings<=20) { fDivY =  5; fDivX =  4; }
            else if (numDrawings<=25) { fDivY =  6; fDivX =  4; }
            else if (numDrawings<=25) { fDivY =  5; fDivX =  5; }
            else if (numDrawings<=30) { fDivY =  6; fDivX =  5; }
            else if (numDrawings<=35) { fDivY =  7; fDivX =  5; }
            else if (numDrawings<=36) { fDivY =  6; fDivX =  6; }
            else if (numDrawings<=40) { fDivY =  8; fDivX =  5; }
            else if (numDrawings<=42) { fDivY =  7; fDivX =  6; }
            else if (numDrawings<=48) { fDivY =  8; fDivX =  6; }
            else if (numDrawings<=63) { fDivY =  9; fDivX =  7; }
            else if (numDrawings<=80) { fDivY = 10; fDivX =  8; }
            else {
                lk_error << "Too many drawings!!! " << numDrawings << endl;
                return false;
            }
        }

        else
        {
            if      (numDrawings== 1) { fDivX =  1; fDivY =  1; }
            else if (numDrawings<= 2) { fDivX =  2; fDivY =  1; }
            else if (numDrawings<= 4) { fDivX =  2; fDivY =  2; }
            else if (numDrawings<= 6) { fDivX =  3; fDivY =  2; }
            else if (numDrawings<= 8) { fDivX =  4; fDivY =  2; }
            else if (numDrawings<= 9) { fDivX =  3; fDivY =  3; }
            else if (numDrawings<=12) { fDivX =  4; fDivY =  3; }
            else if (numDrawings<=16) { fDivX =  4; fDivY =  4; }
            else if (numDrawings<=20) { fDivX =  5; fDivY =  4; }
            else if (numDrawings<=25) { fDivX =  6; fDivY =  4; }
            else if (numDrawings<=25) { fDivX =  5; fDivY =  5; }
            else if (numDrawings<=30) { fDivX =  6; fDivY =  5; }
            else if (numDrawings<=35) { fDivX =  7; fDivY =  5; }
            else if (numDrawings<=36) { fDivX =  6; fDivY =  6; }
            else if (numDrawings<=40) { fDivX =  8; fDivY =  5; }
            else if (numDrawings<=42) { fDivX =  7; fDivY =  6; }
            else if (numDrawings<=48) { fDivX =  8; fDivY =  6; }
            else if (numDrawings<=63) { fDivX =  9; fDivY =  7; }
            else if (numDrawings<=80) { fDivX = 10; fDivY =  8; }
            else {
                lk_error << "Too many drawings!!! " << numDrawings << endl;
                return false;
            }
        }
    }

    double resize_factor = 0.5;
    if      (fDivX>=5||fDivY>=5) resize_factor = 1.00;
    else if (fDivX>=4||fDivY>=4) resize_factor = 0.95;
    else if (fDivX>=3||fDivY>=3) resize_factor = 0.80;
    else if (fDivX>=2||fDivY>=2) resize_factor = 0.65;

    if (fCvs==nullptr)
    {
        if (!fFixCvsSize)
        {
            if (fDXCvs==0 || fDYCvs==0) {
                fDXCvs = 800*fDivX;
                fDYCvs = 680*fDivY;
            }
            fCvs = LKPainter::GetPainter() -> CanvasResize(Form("c%s",fName.Data()), fDXCvs, fDYCvs, resize_factor);
        }
        else
            fCvs = new TCanvas(Form("c%s",fName.Data()),Form("c%s",fName.Data()), fDXCvs, fDYCvs);
    }

    if (fPadArray!=nullptr)
    {
        auto numPads = fPadArray -> GetEntries();
        for (auto iPad=0; iPad<numPads; ++iPad) {
            auto pad = fPadArray -> At(iPad);
            fCvs -> cd();
            pad -> Draw();
        }
        return true;
    }

    if (numDrawings==1)
        return true;

    TObject *obj;
    int nPads = 0;
    TIter next(fCvs -> GetListOfPrimitives());
    while ((obj=next())) {
        if (obj->InheritsFrom(TVirtualPad::Class()))
            nPads++;
    }
    if (nPads<numDrawings) {
        //fCvs -> Divide(fDivX, fDivY, 0.001, 0.001);
        DividePad(fCvs,fDivX, fDivY, 0.001, 0.001);
    }

    return true;
}

void LKDrawingGroup::DividePad(TPad* cvs, Int_t nx, Int_t ny, Float_t xmargin, Float_t ymargin, Int_t color)
{
    cvs -> cd();
    if (nx <= 0) nx = 1;
    if (ny <= 0) ny = 1;
    Int_t ix, iy;
    Double_t x1, y1, x2, y2, dx, dy;
    TPad *pad;
    TString name, title;
    Int_t n = 0;
    //if (color == 0) color = GetFillColor();
    //if (xmargin > 0 && ymargin > 0)
    dy = 1/Double_t(ny);
    dx = 1/Double_t(nx);
    if (CheckOption("vertical_pad_numbering")) {
        //for (ix=0;ix<nx;ix++) {
        for (ix=nx-1;ix>=0;ix--) {
            x2 = 1 - ix*dx - xmargin;
            x1 = x2 - dx + 2*xmargin;
            if (x1 < 0) x1 = 0;
            if (x1 > x2) continue;
            for (iy=ny-1;iy>=0;iy--) {
                y1 = iy*dy + ymargin;
                y2 = y1 +dy -2*ymargin;
                if (y1 > y2) continue;
                n++;
                name.Form("%s_%d", GetName(), n);
                pad = new TPad(name.Data(), name.Data(), x1, y1, x2, y2, color);
                pad->SetNumber(n);
                pad->Draw();
            }
        }
    }
    else { //general case
        for (iy=0;iy<ny;iy++) {
            y2 = 1 - iy*dy - ymargin;
            y1 = y2 - dy + 2*ymargin;
            if (y1 < 0) y1 = 0;
            if (y1 > y2) continue;
            for (ix=0;ix<nx;ix++) {
                x1 = ix*dx + xmargin;
                x2 = x1 +dx -2*xmargin;
                if (x1 > x2) continue;
                n++;
                name.Form("%s_%d", GetName(), n);
                pad = new TPad(name.Data(), name.Data(), x1, y1, x2, y2, color);
                pad->SetNumber(n);
                pad->Draw();
            }
        }
    }
    cvs -> Modified();
}

bool LKDrawingGroup::AddFile(TFile* file, TString groupSelection)
{
    if (fFileName.IsNull())
        fFileName = file -> GetName();
    else
        fFileName = fFileName + ", " + file->GetName();
    SetName(fFileName);

    bool allowPrint = true;
    if (groupSelection=="xprint") {
        allowPrint = false;
        groupSelection = "";
    }

    if (!groupSelection.IsNull())
    {
        vector<TString> groupNameArray;
        if (groupSelection.Index(":")<0)
            groupNameArray.push_back(groupSelection);
        else {
            TObjArray *tokens = groupSelection.Tokenize(":");
            auto n = tokens -> GetEntries();
            for (auto i=0; i<n; ++i) {
                TString token0 = ((TObjString*)tokens->At(i))->GetString();
                groupNameArray.push_back(token0);
            }
        }
        for (auto groupName : groupNameArray)
        {
            if (groupName.EndsWith("*"))
            {
                groupName.Remove(groupName.Sizeof()-2);
                auto listOfKeys = file -> GetListOfKeys();
                TKey* key = nullptr;
                TIter nextKey(listOfKeys);
                while ((key=(TKey*)nextKey())) {
                    if (TString(key->GetName()).Index(groupName)==0) {
                        auto group = (LKDrawingGroup*) key -> ReadObj();
                        if (allowPrint) e_info << "Adding " << group->GetName() << " from " << file->GetName() << endl;
                        AddGroupInStructure(group);
                    }
                }
            }
            else
            {
                auto obj = file -> Get(groupName);
                if (obj==nullptr) {
                    e_error << groupName << " is nullptr" << endl;
                    continue;
                }
                if (TString(obj->ClassName())=="LKDrawingGroup") {
                    auto group = (LKDrawingGroup*) obj;
                    if (allowPrint) e_info << "Adding " << group->GetName() << " from " << file->GetName() << endl;
                    AddGroupInStructure(group);
                }
                else {
                    e_error << groupName << " type is not LKDrawingGroup" << endl;
                    return false;
                }
            }
        }
    }
    else
    {
        auto listOfKeys = file -> GetListOfKeys();
        TKey* key = nullptr;
        TIter nextKey(listOfKeys);
        while ((key=(TKey*)nextKey())) {
            if (TString(key->GetClassName())=="LKDrawingGroup") {
                auto group = (LKDrawingGroup*) key -> ReadObj();
                if (allowPrint) e_info << "Adding " << group->GetName() << " from " << file->GetName() << endl;
                AddGroupInStructure(group);
            }
        }
    }
    return true;
}

bool LKDrawingGroup::AddFile(TString fileName, TString groupSelection)
{
    TFile* file = new TFile(fileName);
    if (!file->IsOpen()) {
        e_error << fileName << " is cannot be openned" << endl;
        return false;
    }
    return AddFile(file,groupSelection);
}

int LKDrawingGroup::GetNumGroups() const
{
    if (fIsGroupGroup)
        return GetEntries();
    return 0;
}

bool LKDrawingGroup::CheckIsGroupGroup(bool add)
{
    if (GetEntries()==0) {
        if (add) fIsGroupGroup = true;
        return true;
    }
    if (!fIsGroupGroup) {
        if (add) lk_error << "group is drawing-group" << endl;
        return false;
    }
    return true;
}

bool LKDrawingGroup::CheckIsDrawingGroup(bool add)
{
    if (GetEntries()==0) {
        if (add) fIsGroupGroup = false;
        return true;
    }
    if (fIsGroupGroup) {
        if (add) lk_error << "group is group-group" << endl;
        return false;
    }
    return true;
}

int LKDrawingGroup::GetGroupDepth() const
{
    if (fIsGroupGroup) {
        auto maxDepth = 0;
        auto numSub = GetEntries();
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            auto depth = sub -> GetGroupDepth();
            if (depth>maxDepth)
                maxDepth = depth;
        }
        return maxDepth + 1;
    }
    else
        return 1;
}

TString LKDrawingGroup::GetFullName() const
{
    if (fParentName.IsNull())
        return fName;
    else {
        return fParentName+":"+fName;
    }
}

void LKDrawingGroup::Add(TObject *obj)
{
    if (obj->InheritsFrom(TH1::Class())) { AddHist((TH1*) obj); return; }
    if (obj->InheritsFrom(TGraph::Class())) { AddGraph((TGraph*) obj); return; }
    if (obj->InheritsFrom(LKDrawing::Class()) || obj->InheritsFrom(LKDrawingGroup::Class()))
        TObjArray::Add(obj);
    else
        lk_error << "Cannot add " << obj->ClassName() << " directly!" << endl;
}

void LKDrawingGroup::AddDrawing(LKDrawing* drawing)
{
    if (CheckIsDrawingGroup(true))
        Add(drawing);
}

void LKDrawingGroup::AddGraph(TGraph* graph, TString drawOption, TString title)
{
    if (CheckIsDrawingGroup(true)) {
        auto drawing = new LKDrawing();
        drawing -> Add(graph,drawOption,title);
        Add(drawing);
    }
}

void LKDrawingGroup::AddHist(TH1 *hist, TString drawOption, TString title)
{
    if (CheckIsDrawingGroup(true)) {
        auto drawing = new LKDrawing();
        drawing -> Add(hist,drawOption,title);
        Add(drawing);
    }
}

int LKDrawingGroup::GetNumDrawings() const
{
    if (!fIsGroupGroup)
        return GetEntries();
    return 0;
}

int LKDrawingGroup::GetNumAllGroups() const
{
    int numDrawings = 0;
    if (fIsGroupGroup)
    {
        auto numSub = GetEntries();
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            numDrawings += sub -> GetNumAllGroups();
        }
    }
    else
        numDrawings = 1;

    return numDrawings;
}

int LKDrawingGroup::GetNumAllDrawings() const
{
    int numDrawings = 0;
    if (fIsGroupGroup)
    {
        auto numSub = GetEntries();
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            numDrawings += sub -> GetNumAllDrawings();
        }
    }
    else
        numDrawings += GetEntries();

    return numDrawings;
}

int LKDrawingGroup::GetNumAllDrawingObjects() const
{
    int numObjects = 0;
    if (fIsGroupGroup)
    {
        auto numSub = GetEntries();
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            numObjects += sub -> GetNumAllDrawingObjects();
        }
    }
    else {
        auto numDrawing = GetEntries();
        for (auto iDrawing=0; iDrawing<numDrawing; ++iDrawing) {
            auto drawing = (LKDrawingGroup*) At(iDrawing);
            numObjects += drawing -> GetEntries();
        }
    }

    return numObjects;
}

void LKDrawingGroup::AddGroupInStructure(LKDrawingGroup *group)
{
    auto parentLevel = group -> GetGroupLevel() - 1;
    auto parentName = group -> GetParentName();
    if (parentLevel==0)
    {
        AddGroup(group);
    }
    else
    {
        vector<TString> parentNameArray;
        parentName = Form(" %s ",parentName.Data());
        TObjArray *tokens = parentName.Tokenize(":");
        auto n = tokens -> GetEntries();
        for (auto i=0; i<n; ++i) {
            TString token0 = ((TObjString*)tokens->At(i))->GetString();
            token0.ReplaceAll(" ","");
            parentNameArray.push_back(token0);
        }
        if (parentLevel==1) {
            TString pname = parentNameArray.at(1);
            LKDrawingGroup* pGroup = FindGroup(pname);
            if (pGroup==nullptr)
                pGroup = CreateGroup(pname);
            pGroup -> AddGroup(group);
        }
    }
}

void LKDrawingGroup::AddGroup(LKDrawingGroup *group)
{
    if (CheckIsGroupGroup(true)) {
        group -> SetGroupLevel(fGroupLevel+1);
        TString fullName = GetFullName();
        group -> SetParentName(fullName);
        Add(group);
    }
}

LKDrawingGroup* LKDrawingGroup::CreateGroup(TString name, bool addToList)
{
    if (CheckIsGroupGroup(true)) {
        if (name.IsNull()) name = Form("%s_%d",fName.Data(),GetEntries());
        auto sub = new LKDrawingGroup(name,fGroupLevel+1);
        if (addToList) AddGroup(sub);
        return sub;
    }
    return (LKDrawingGroup*) nullptr;
}

LKDrawingGroup* LKDrawingGroup::GetGroup(int iSub)
{
    if (CheckIsDrawingGroup())
        return (LKDrawingGroup*) nullptr;
    if (iSub<0 || iSub>=GetEntries()) {
        return (LKDrawingGroup*) nullptr;
    }
    auto subGroup = (LKDrawingGroup*) At(iSub);
    return subGroup;
}

LKDrawing* LKDrawingGroup::GetDrawing(int iDrawing)
{
    if (CheckIsGroupGroup())
        return (LKDrawing*) nullptr;
    if (iDrawing<0 || iDrawing>=GetEntries()) {
        return (LKDrawing*) nullptr;
    }
    auto drawing = (LKDrawing*) At(iDrawing);
    return drawing;
}

LKDrawing* LKDrawingGroup::CreateDrawing(TString name, bool addToList)
{
    if (CheckIsDrawingGroup(true)) {
        auto drawing = new LKDrawing(name);
        if (addToList) Add(drawing);
        return drawing;
    }
    return (LKDrawing*) nullptr;
}

LKDrawingGroup* LKDrawingGroup::FindGroup(TString name, int option)
{
    if (fIsGroupGroup)
    {
        auto numSub = GetEntries();
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            TString groupSelection = sub -> GetName();
            if (option==2)
            {
                if (name.Index(groupSelection)>=0)
                    return sub;
            }
            else {
                if (option==1)
                    groupSelection = sub -> GetFullName();
                if (groupSelection==name)
                    return sub;
            }
            auto found = sub -> FindGroup(name, option);
            if (found!=nullptr)
                return found;
        }
    }
    return ((LKDrawingGroup *) nullptr);
}

bool LKDrawingGroup::CheckGroup(LKDrawingGroup *find)
{
    if (fIsGroupGroup)
    {
        auto numSub = GetEntries();
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            if (sub==find)
                return true;
            auto found = sub -> CheckGroup(find);
            if (found)
                return true;
        }
    }
    return false;
}

LKDrawing* LKDrawingGroup::FindDrawing(TString name)
{
    if (CheckIsGroupGroup())
    {
        auto numSub = GetEntries();
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            LKDrawing* drawing = sub -> FindDrawing(name);
            if (drawing!=nullptr)
                return drawing;
        }
    }
    else
    {
        auto numDrawings = GetEntries();
        for (auto iDrawing=0; iDrawing<numDrawings; ++iDrawing)
        {
            auto drawing = (LKDrawing*) At(iDrawing);
            if (drawing->GetName()==name)
                return drawing;
        }
    }
    return (LKDrawing*) nullptr;
}

TH1* LKDrawingGroup::FindHist(TString name)
{
    if (CheckIsGroupGroup())
    {
        auto numSub = GetEntries();
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            TH1* hist = sub -> FindHist(name);
            if (hist!=nullptr)
                return hist;
        }
    }
    else
    {
        auto numDrawings = GetEntries();
        for (auto iDrawing=0; iDrawing<numDrawings; ++iDrawing)
        {
            auto drawing = (LKDrawing*) At(iDrawing);
            auto numObjs = drawing -> GetEntries();
            for (auto iObj=0; iObj<numObjs; ++iObj)
            {
                auto obj = drawing -> At(iObj);
                if (obj->InheritsFrom(TH1::Class()))
                {
                    if (obj->GetName()==name) {
                        return (TH1*) obj;
                    }
                }
            }
        }
    }
    return (TH1*) nullptr;
}

TGraph* LKDrawingGroup::FindGraph(TString name)
{
    if (CheckIsGroupGroup())
    {
        auto numSub = GetEntries();
        for (auto iSub=0; iSub<numSub; ++iSub) {
            auto sub = (LKDrawingGroup*) At(iSub);
            TGraph* graph = sub -> FindGraph(name);
            if (graph!=nullptr)
                return graph;
        }
    }
    else
    {
        auto numDrawings = GetEntries();
        for (auto iDrawing=0; iDrawing<numDrawings; ++iDrawing)
        {
            auto drawing = (LKDrawing*) At(iDrawing);
            auto numObjs = drawing -> GetEntries();
            for (auto iObj=0; iObj<numObjs; ++iObj)
            {
                auto obj = drawing -> At(iObj);
                if (obj->InheritsFrom(TGraph::Class()))
                {
                    if (obj->GetName()==name) {
                        return (TGraph*) obj;
                    }
                }
            }
        }
    }
    return (TGraph*) nullptr;
}
