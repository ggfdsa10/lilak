void daqRun()
{
    auto runManager = new STDRunManager();
    runManager -> AddDetector(new TPCDrum());
    runManager -> SetDAQStage(); // for decoding
    runManager -> AddPar("TPCDrum_Reco.mac");
    runManager -> SetSiArrayAsAdID(3);

    TString runNumber = "261118004";
    runManager -> SetRunList(runNumber); // Only avaliable for Sejong QCD2 server

    // TString dataFolderPath = "/data/public_data/daq/260625/260625_AsAd0Only/default/";
    // runManager ->  SetDatapath(dataFolderPath); // temporary open the data with data folder path
    // runManager -> SetEventNumber(1000);

    STDNoiseSubtractor* noise = new STDNoiseSubtractor();
    STDChannelViewer* viewer = new STDChannelViewer();
    viewer -> SetRunNumber(runNumber);
    viewer -> OnEventFigure();
    // runManager -> Add(noise);
    runManager -> Add(viewer);

    runManager -> Init();
    runManager -> Run();
}