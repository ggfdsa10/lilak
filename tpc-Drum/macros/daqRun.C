void daqRun()
{
    auto runManager = new LKRunManager();
    runManager -> AddDetector(new TPCDrum());
    runManager -> SetDAQStage(); // for decoding
    runManager -> SetRunList("240824003");

    // runManager -> SetEventNumber(1000);

    STDNoiseSubtractor* noiseSubtractor = new STDNoiseSubtractor();
    STDPulseAnalyser* pulseAnal = new STDPulseAnalyser();

    runManager -> Add(noiseSubtractor);
    runManager -> Add(pulseAnal);

    runManager -> Init();
    runManager -> Run();
}