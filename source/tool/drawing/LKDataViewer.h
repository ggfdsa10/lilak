#ifndef LKDATAVIEWER_HH
#define LKDATAVIEWER_HH

#include "TGGC.h"
#include "TGTab.h"
#include "TGFont.h"
#include "TGLabel.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGCanvas.h"
#include "TGListBox.h"
#include "TGTextEntry.h"
#include "TGNumberEntry.h"
#include "TRootEmbeddedCanvas.h"

#include "LKRun.h"
#include "LKDrawing.h"
#include "LKDrawingGroup.h"

class LKDataViewer : public TGMainFrame
{
    protected:
        bool fInitialized = false;
        int fInitWidth, fInitHeight;
        double fControlFrameXRatio = 0.15;
        double fStatusFrameYRatio = 0.05;
        double fRF = 1; ///< Resize factor (scale factor of your screen compare to the mornitor which has with of 1500)
        double fRFEntry = 0.8; ///< Resize factor for number entry
        double fResizeFactorX = 1.;
        double fResizeFactorY = 1.;
        int fWindowSizeX = 0;
        int fWindowSizeY = 0;
        TString fSavePath = "data_lilak";
        bool fMinimumUIComponents = false;
        bool fIsActive = false;
        int fCanvasFillColor = 0;

        LKDrawingGroup *fCurrentGroup = nullptr;
        LKDrawing *fCurrentDrawing = nullptr;
        TCanvas *fCurrentCanvas = nullptr;
        TVirtualPad *fCurrentTPad = nullptr;
        int fSelectColor = kGray;

        TGFont*      fGFont1;
        TGFont*      fGFont2;
        TGFont*      fGFont3;
        FontStruct_t fSFont1;
        FontStruct_t fSFont2;
        FontStruct_t fSFont3;

        TGHorizontalFrame *fMainFrame = nullptr;
        TGVerticalFrame *fControlFrame = nullptr;
        TGVerticalFrame *fStatusFrame = nullptr;
        TGHorizontalFrame *fBottomFrame = nullptr;

        TGNumberEntry *fEventNumberEntry = nullptr;
        TGNumberEntry *fEventRangeEntry1 = nullptr;
        TGNumberEntry *fEventRangeEntry2 = nullptr;
        TGNumberEntryField *fNumberInput; // Input field for the number pad
        //std::vector<TGTextButton *> fNumberButtons; // Buttons for the number pad

        TGGroupFrame* fNavControlSection = nullptr;
        TGTextButton* fButton_H = nullptr;
        TGTextButton* fButton_L = nullptr;
        TGTextButton* fButton_J = nullptr;
        TGTextButton* fButton_K = nullptr;
        TGTextButton* fButton_T = nullptr;
        TGTextButton* fButton_U = nullptr;

        int fCurrentCanvasX = 0;
        int fCurrentCanvasY = 0;
        bool fPublicGroupIsAdded = false;
        LKDrawingGroup *fPublicGroup = nullptr;
        int fCountPublicSub = 0;
        int fPublicTabIndex = 0;

        bool fUseTRootCanvas = false;
        TGTab *fTabSpace = nullptr;
        TGTab *fCurrentSubTabSpace = nullptr;
        TGListBox* fTabListBox = nullptr;
        LKDrawingGroup *fTopDrawingGroup = nullptr;
        vector<bool> fTabShouldBeUpdated;
        vector<LKDrawingGroup*> fTabGroup;
        vector<vector<bool>> fSubTabShouldBeUpdated;
        vector<vector<LKDrawingGroup*>> fSubTabGroup;
        vector<TGTab*> fSubTabSpace;
        vector<int> fNumSubTabs;
        int fCurrentTabID = 0;
        int fCurrentSubTabID = 0;
        int fSaveTabID = 0;
        int fSaveSubTabID = 0;
        bool fLayoutButIgnoreSelectedSignal = false;

        int fCountMessageUpdate = 0;
        //TGLabel* fStatusMessages[3];
        TGLabel* fStatusMessages[2];
        //TGLabel* fStatusDataName = nullptr;
        //TGTextView *fMessageHistoryView = nullptr;

        TString fTitle;
        TString fDrawOption;

        LKRun *fRun = nullptr;

    public:
        LKDataViewer(const TGWindow *p=nullptr, UInt_t w=1500, UInt_t h=800);
        LKDataViewer(LKDrawingGroup *top, const TGWindow *p=nullptr, UInt_t w=1500, UInt_t h=800);
        LKDataViewer(LKDrawing *drawing, const TGWindow *p=nullptr, UInt_t w=1500, UInt_t h=800);
        LKDataViewer(TString fileName, TString groupSelection="", const TGWindow *p=nullptr, UInt_t w=1500, UInt_t h=800);
        LKDataViewer(TFile* file, TString groupSelection="", const TGWindow *p=nullptr, UInt_t w=1500, UInt_t h=800);
        virtual ~LKDataViewer();

        void SetUseTRootCanvas(bool value) { fUseTRootCanvas = value; }
        void AddDrawing(LKDrawing* drawing);
        void AddGroup(LKDrawingGroup* group, bool addDirect=false);
        bool AddFile(TFile* file, TString groupSelection="");
        bool AddFile(TString file, TString groupSelection="");
        void SetRun(LKRun* run) { fRun = run; }

        LKDrawingGroup* GetTopDrawingGroup() const { return fTopDrawingGroup; }

        /// resize
        /// saveall
        void Draw(TString option="");
        virtual void Print(Option_t *option="") const;
        virtual void SetName(const char* name);

        bool IsActive() const { return fIsActive; }

    protected:
        bool InitParameters();
        bool InitFrames();

        TGLayoutHints* NewHintsMainFrame();
        TGLayoutHints* NewHintsFrame();
        TGLayoutHints* NewHintsInnerFrame();
        TGLayoutHints* NewHintsTopFrame();
        TGLayoutHints* NewHintsInnerButton();
        TGLayoutHints* NewHintsMinimumUI();
        TGLayoutHints* NewHints(int option);

        TGGroupFrame* NewGroupFrame(TString sectionName);
        TGHorizontalFrame* NewHorizontalFrame(TGGroupFrame* section);

        int AddGroupTab(LKDrawingGroup* group, int iTab=-1, int iSub=-1); ///< CreateMainCanvas

        void CreateMainFrame();
        void CreateMainCanvas();
        void CreateStatusFrame();
        void CreateControlFrame();
        void CreateEventControlSection(); ///< CreateControlFrame
        void CreateEventRangeControlSection(); ///< CreateControlFrame
        void CreateChangeControlSection();
        void CreateViewerControlSection(); ///< CreateControlFrame
        void CreateCanvasControlSection(); ///< CreateControlFrame
        void CreateTabControlSection(); ///< CreateControlFrame
        void CreateNumberPad(); ///< CreateControlFrame

        /// messageType 0:default 1:info 2:warning
        void SendOutMessage(TString message, int messageType=0, bool printScreen=false);

    public:
        void ProcessPrevEvent();
        void ProcessNextEvent();
        void ProcessGotoEvent();
        void ProcessAllEvents();
        void ProcessRangeEvents();
        void ProcessExitViewer();
        void ProcessSaveTab(int ipad);
        void ProcessGotoTopTab(int iTab=-1, int iSub=-1, bool layout=true, int signal=0);
        void ProcessLayoutTopTab(int iTab=-1, int iSub=-1);
        void ProcessLayoutSubTab(int iSub=-1);
        //void ProcessGotoTopTabClicked();
        //void ProcessGotoTopTabSelected(Int_t id);
        void ProcessGotoSubTab(int iSub=-1, bool layout=true);
        void ProcessPrevTab();
        void ProcessNextTab();
        void ProcessPrevSubTab();
        void ProcessNextSubTab();
        void ProcessAccumulateEvents() {}
        void ProcessTabSelection(Int_t id);
        void ProcessLoadAllCanvas();
        void ProcessReLoadCCanvas();
        void ProcessReLoadACanvas();
        void ProcessTCutEditorMode(int iMode=-1);
        void ProcessWaitPrimitive(int iMode);
        void ProcessCanvasControl(int iMode);
        void ProcessNavigationMode(int iMode);
        void ProcessNavigateCanvas(int iMode);
        void ProcessToggleNavigateCanvas();
        void ProcessUndoToggleCanvas();
        void ProcessSizeViewer(double scale=1, double scaley=1);

        void HandleNumberInput(Int_t id);

    ClassDef(LKDataViewer,1)
};

#endif
