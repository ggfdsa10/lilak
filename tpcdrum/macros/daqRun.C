void daqRun()
{
    auto runManager = new STDRunManager();
    runManager -> AddDetector(new TPCDrum());
    runManager -> SetDAQStage(); // for decoding
    runManager -> AddPar("TPCDrum_Reco.mac");
    runManager -> SetSiArrayAsAdID(3);

    TString runNumber = "260626007";
    runManager -> SetRunList(runNumber); // Only avaliable for Sejong QCD2 server

    // TString dataFolderPath = "";
    // runManager ->  SetDatapath(dataFolderPath); // temporary open the data with data folder path



    runManager -> SetEventNumber(1000);


    STDNoiseSubtractor* noiseSubtractor = new STDNoiseSubtractor();
    STDChannelViewer* viewer = new STDChannelViewer();
    viewer -> SetRunNumber(runNumber);
    // viewer -> OnEventFigure();

    runManager -> Add(noiseSubtractor);
    runManager -> Add(viewer);

    runManager -> Init();
    runManager -> Run();
}