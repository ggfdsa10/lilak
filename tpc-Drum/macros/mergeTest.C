void mergeTest()
{
    // const int runListNum = 1;
    // TString runFile[runListNum] = {""};

    // TFile* file[runListNum];

    LKRun* run;
    for(int i=0; i<2; i++){
        // file[i] = new TFile(Form("./test_%s.root", runFile[i].Data()), "read");
        run = new LKRun();

        run -> Init();

        delete run;
        run = 0;
    }
}