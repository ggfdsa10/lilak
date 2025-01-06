void hitAnal()
{
    auto runManager = new STDRunManager();
    runManager -> AddDetector(new TPCDrum());
    // runManager -> SetDataPath("/home/shlee/workspace/lilak/tpc_Drum/macros/");
    // runManager -> SetRunList("240823004 240824001 240824002");// AsAd Full, 2100V 

    runManager -> SetRunList("250102001");// AsAd 0, 2000V 

    STDEventViewer* viewer = new STDEventViewer();
    viewer->SetRunNumber("250102001");
    runManager -> Add(viewer);

    runManager -> Init();
    runManager -> Run();
}