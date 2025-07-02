void makeSimData()
{
    auto run = new LKRun();
    run -> AddDetector(new TPCDrum());
    run -> SetInputFile("./simTest.root");

    run -> Add(new STDDriftElectronMaker);
    run -> Add(new STDElectronicsMaker);
    run -> Add(new STDNoiseSubtractor);
    run -> Add(new STDPulseAnalyser);
    run -> Add(new STDMCViewer);

    run -> Init();
    run -> Run();
}