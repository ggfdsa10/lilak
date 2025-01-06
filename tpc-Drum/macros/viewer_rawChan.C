void viewer_rawChan()
{
    auto runManager = new LKRunManager();
    runManager -> AddDetector(new TPCDrum());
    runManager -> SetDAQStage(); // for decoding
    runManager -> SetRunList("241014002");
    // runManager -> SetEventNumber(1200);

    STDChannelViewer* chanViewer = new STDChannelViewer();

    runManager -> Add(chanViewer);

    runManager -> Init();
    runManager -> Run();
}