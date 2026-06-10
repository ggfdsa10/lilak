void makeSimData()
{
    auto run = new LKRun();
    run -> AddDetector(new TPCDrum());
    // run -> SetInputFile("./simTest.root");
    run -> SetInputFile("./test.root");
    // run -> SetInputFile("/home/shlee/workspace/lilak/data/run_0000.root");

    // run -> Add(new STDDriftElectronMaker); // TPC reponse
    run -> Add(new STDSiResponseMaker);   // Si detector response

    run -> Add(new STDElectronicsMaker);

    // STDNoiseSubtractor* subtract = new STDNoiseSubtractor();
    // subtract -> DrawRawADCPad();
    // run -> Add(subtract);
    // run -> Add(new STDPulseAnalyser);
    // run -> Add(new STDMCViewer);

    run -> Init();
    run -> Run();
}