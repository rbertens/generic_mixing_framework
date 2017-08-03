void runJetFindingOnMixedEvents(TString fileName = "myMixedEvents.root")
{

    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$FASTJET/include -I$PATH_TO_SOURCE/");

    // load fastjet libraries
//    gSystem->Load("libCGAL");
    gSystem->Load("libfastjet");
    gSystem->Load("libsiscone");
    gSystem->Load("libsiscone_spherical");
    gSystem->Load("libfastjetplugins");
    gSystem->Load("libfastjettools");
    gSystem->Load("libfastjetcontribfragile");

    // compile the encapsulated classes
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFHistogramManager.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFEventContainer.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFEventReader.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFSimpleEventCuts.cxx+");

    // compile the jet finding classes
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFSimpleJetFinder.cxx+");

    TChain* myChain = new TChain("tree");
    myChain->Add(fileName.Data());


    // add more files if desired, e.g. per class

    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    cout << reader->GetNumberOfEvents() << " events available for analysis " << endl;
    
    // create the jet finder
    AliGMFSimpleJetFinder* jetFinder = new AliGMFSimpleJetFinder();
    jetFinder->Initialize();    // tbd pass enum on configuratioin
    
    /* create the event cuts
     *
     * no cuts are used here, the mixed events
     * are tuned specifically to what we want
     */
  //  AliGMFSimpleEventCuts* eventCuts = new AliGMFSimpleEventCuts();
  //  eventCuts->SetMultiplicityRange(190, 210);
  //  eventCuts->SetVertexRange(-5, 5);
  //  eventCuts->SetEventPlaneRange(-10, 10);
  //  eventCuts->SetCentralityRange(10, 12);

    // pass the event cuts to the jet finder
  //  jetFinder->SetEventCuts(eventCuts);
    

    TStopwatch timer;
    timer.Start();
    
    Int_t iEvents = reader->GetNumberOfEvents();
    Float_t remainingTime = -1;

    // set max number of accepted events
    Int_t iMaxEvents = 23000;

    for (int i = 0, j = 0 ; i < iEvents; i ++) {
        if(i==100) {
            remainingTime = timer.RealTime()/100.;
            cout << " - remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl;
        } else if (i > 0 && i%1000 == 0) cout << " - remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl; 
        if(!jetFinder->AnalyzeEvent(reader->GetEvent(i))) continue;
        j++;
        if(j > iMaxEvents) break;
        cout << " Processed event " << i << " of which accepted " << j << "\r"; cout.flush();
    }

    // write and clear memory
    jetFinder->Finalize("myMixedJets_peripheral");

    delete jetFinder;
    delete reader;

}
