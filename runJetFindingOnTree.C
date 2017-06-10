void runJetFindingOnTree()
{

    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include -I$FASTJET/include");

    // load fastjet libraries
    gSystem->Load("libCGAL");
    gSystem->Load("libfastjet");
    gSystem->Load("libsiscone");
    gSystem->Load("libsiscone_spherical");
    gSystem->Load("libfastjetplugins");
    gSystem->Load("libfastjettools");
    gSystem->Load("libfastjetcontribfragile");

    // compile the encapsulated classes
    gROOT->LoadMacro("AliGMFHistogramManager.cxx+");
    gROOT->LoadMacro("AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("AliGMFEventContainer.cxx+");
    gROOT->LoadMacro("AliGMFEventReader.cxx+");

    // compile the jet finding classes
    gROOT->LoadMacro("AliGMFDummyJetFinder.cxx+");

    TChain* myChain = new TChain("tree");
    myChain->Add("myFilteredTree.root");
    // add more files if desired, e.g. per class

    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    cout << reader->GetNumberOfEvents() << " events available for analysis " << endl;
    
    AliGMFDummyJetFinder* jetFinder = new AliGMFDummyJetFinder();
    jetFinder->Initialize();    // tbd pass enum on configuration

    TStopwatch timer;
    timer.Start();
    
    Int_t iEvents = reader->GetNumberOfEvents();
    Float_t remainingTime = -1;

    for (int i = 0 ; i < iEvents; i ++) {
        jetFinder->AnalyzeEvent(reader->GetEvent(i));
        if(i%10 == 0) cout << " Processed event " << i << "\r"; cout.flush();
        if(i==100) {
            remainingTime = timer.RealTime()/100.;
            cout << "remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl;
        } else if (i > 0 && i%1000 == 0) cout << " remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl; 

    }

    // write and clear memory
    jetFinder->Finalize();

    delete jetFinder;
    delete reader;

}
