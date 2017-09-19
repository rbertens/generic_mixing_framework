void runJetFindingOnTree(
        Int_t cenMin = 0,
        Int_t cenMax = 10,
        Int_t file = 0)
{

    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)

    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$FASTJET/include");

    gSystem->Load("libCGAL");
    gSystem->Load("libfastjet");
    gSystem->Load("libsiscone");
    gSystem->Load("libsiscone_spherical");
    gSystem->Load("libfastjetplugins");
    gSystem->Load("libfastjettools");
    gSystem->Load("libfastjetcontribfragile");

    gROOT->LoadMacro("AliGMFHistogramManager.cxx+");
    gROOT->LoadMacro("AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("AliGMFEventContainer.cxx+");
    gROOT->LoadMacro("AliGMFEventReader.cxx+");
    gROOT->LoadMacro("AliGMFSimpleTrackCuts.cxx+");
    gROOT->LoadMacro("AliGMFSimpleEventCuts.cxx+");
    gROOT->LoadMacro("AliGMFSimpleJetFinder.cxx+");


    // compile the jet finding classes
    gROOT->LoadMacro("AliGMFSimpleJetFinder.cxx+");

    // add the desired numbers to a chain (not exception safe for now!)
    TChain* myChain = new TChain("tree");
    myChain->Add(Form("/eos/user/r/rbertens/sandbox/merge/%i.root", file));

    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    cout << reader->GetNumberOfEvents() << " events available for analysis " << endl;

    Int_t iEvents = reader->GetNumberOfEvents();

    // initialize the jet finder
    AliGMFSimpleJetFinder* jetFinder[4];
    float radii[] = {.2, .3, .4, .5};
     // create the event cuts
    AliGMFSimpleEventCuts* eventCuts = new AliGMFSimpleEventCuts();
    eventCuts->SetCentralityRange(cenMin, cenMax);
    AliGMFSimpleTrackCuts* trackCuts = new AliGMFSimpleTrackCuts();
    trackCuts->SetTrackMinPt(1.);
   
    for(int i = 0; i < 4; i++) {
       jetFinder[i] = new AliGMFSimpleJetFinder();
       jetFinder[i]->SetJetResolution(radii[i]);
       // pass the event cuts to the jet finder
       jetFinder[i]->SetEventCuts(eventCuts);
       jetFinder[i]->SetTrackCuts(trackCuts);
       jetFinder[i]->Initialize();
    }

    for (int i = 0; i < 1000; i ++) {
        for(int j = 0; j < 4; j++) {
            jetFinder[j]->AnalyzeEvent(reader->GetEvent(i));
            cout <<"Event: " << i << "\r"; cout.flush();
        }
    }

    for (int i = 0; i < 4; i++) {
        jetFinder[i]->Finalize(Form("SE_jets_%i_%i_R0%i", cenMin, cenMax, i+2));
        delete jetFinder[i];
    }
    
    delete reader;

}
