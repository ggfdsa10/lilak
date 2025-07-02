void test()
{
    auto run = new LKRun();
    run -> AddDetector(new TPCDrum());
    // run -> SetInputFile("./simTest_bkg.root");
    run -> SetInputFile("./simTest2.root");
    // run -> AddInputFile("./simTest.root");

    run -> Add(new STDTest);

    run -> Init();
    run -> Run();
    // run -> ExecuteEvent(1);
}