void hitAnal()
{
    auto runManager = new LKRunManager();
    runManager -> AddDetector(new TPCDrum());

    runManager -> SetDataPath("/home/shlee/workspace/lilak/tpc_Drum/macros/");
    runManager -> SetRunList("fileList.list");

    STDEventViewer* viewer = new STDEventViewer();
    runManager -> Add(viewer);

    runManager -> Init();
    runManager -> Run();
}