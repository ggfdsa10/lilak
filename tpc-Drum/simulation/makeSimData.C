void makeSimData()
{
    auto run = new LKRun();
    run -> AddDetector(new TPCDrum());
    run -> SetInputFile("./simTest.root");

    STDSimTuningManager* simTunManager = new STDSimTuningManager();
    STDDriftElectronMaker* deMaker = new STDDriftElectronMaker();

    
    run -> Add(simTunManager);
    run -> Add(deMaker);

    run -> Init();
    run -> Run();
}