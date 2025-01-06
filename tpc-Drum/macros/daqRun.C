void daqRun()
{
    auto runManager = new STDRunManager();
    runManager -> AddDetector(new TPCDrum());
    runManager -> SetDAQStage(); // for decoding
    runManager -> SetRunList("241014002");

    STDNoiseSubtractor* noiseSubtractor = new STDNoiseSubtractor();
    STDPulseAnalyser* pulseAnal = new STDPulseAnalyser();

    runManager -> Add(noiseSubtractor);
    runManager -> Add(pulseAnal);

    runManager -> Init();
    runManager -> Run();
}