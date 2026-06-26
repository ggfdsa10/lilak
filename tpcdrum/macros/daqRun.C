void daqRun()
{
    auto runManager = new STDRunManager();
    runManager -> AddDetector(new TPCDrum());
    runManager -> SetDAQStage(); // for decoding
    runManager -> AddPar("TPCDrum_Reco.mac");
    // runManager -> SetSiArrayAsAdID(0);

    TString runNumber = "260509001";



    runManager -> SetRunList(runNumber);
    

    STDNoiseSubtractor* noiseSubtractor = new STDNoiseSubtractor();
    STDChannelViewer* viewer = new STDChannelViewer();
    viewer -> SetRunNumber(runNumber);

    runManager -> Add(noiseSubtractor);
    runManager -> Add(viewer);

    runManager -> Init();
    runManager -> Run();
}