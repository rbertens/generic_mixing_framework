void runFullProcess()
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
    gROOT->LoadMacro("AliGMFMixingManager.cxx+");

    // compile the jet finding classes
    gROOT->LoadMacro("AliGMFSimpleJetFinder.cxx+");

    TChain* myChain = new TChain("tree");
    myChain->Add("myFilteredTree.root");
    // add more files if desired, e.g. per class

    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    cout << reader->GetNumberOfEvents() << " events available for analysis " << endl;
/*    
    AliGMFSimpleJetFinder* jetFinder = new AliGMFSimpleJetFinder();
    jetFinder->Initialize();    // tbd pass enum on configuration

    TStopwatch timer;
    timer.Start();
    
    Int_t iEvents = reader->GetNumberOfEvents();
    Float_t remainingTime = -1;

    // run mixing on unmixed events
    for (int i = 0 ; i < 1000; i ++) {
        jetFinder->AnalyzeEvent(reader->GetEvent(i));
        cout << " Processed event " << i << "\r"; cout.flush();
        if(i==100) {
            remainingTime = timer.RealTime()/100.;
            cout << " - remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl;
        } else if (i > 0 && i%1000 == 0) cout << " - remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl; 
    }

    // write and clear memory
    jetFinder->Finalize("unmixed");

    delete jetFinder;
    delete reader;

*/
    // create the mixer and connect the input event reader
    AliGMFMixingManager* mixer = new AliGMFMixingManager();
    mixer->SetEventReader(reader);

    // configure the mixer
    mixer->SetMultiplicityRange(100, 200);
    mixer->SetVertexRange(-5, 5);
    mixer->SetEventPlaneRange(-10, 10);
    mixer->DoQA();

    // run the mixer
    mixer->DoPerChunkMixing();
    // sit back and enjoy the results


    // do jetfinding on the mixed stuff
    TChain* myMixedChain = new TChain("tree");
    myMixedChain->Add("myMixedEvents.root");
    // add more files if desired, e.g. per class

    // initialize the reader and jet finder
    AliGMFEventReader* mixedReader = new AliGMFEventReader(myMixedChain);
    AliGMFSimpleJetFinder* mixedJetFinder = new AliGMFSimpleJetFinder();
    mixedJetFinder->Initialize();    // tbd pass enum on configuration

    iEvents = mixedReader->GetNumberOfEvents();

    // run mixing on unmixed events
    for (int i = 0 ; i < iEvents; i ++) {
        mixedJetFinder->AnalyzeEvent(mixedReader->GetEvent(i));
        cout << " Processed event " << i << "\r"; cout.flush();
    }
    // write and clear memory
    mixedJetFinder->Finalize("mixed");

    delete mixedjetFinder;
    delete mixedReader;

}
