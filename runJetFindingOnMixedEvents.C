void runJetFindingOnMixedEvents(Int_t fileSuffix = 0,
        Float_t minConstPt = 0.,
        Float_t leadingHadronPt = 0,
        Float_t leadingHadronMaxPt = 1e9,
        Float_t splitTracksFrom = 1e9,
        Float_t splitThemIn = 0,
        Bool_t randomize = kFALSE)
{
    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$FASTJET/include");

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
    gROOT->LoadMacro("AliGMFSimpleEventCuts.cxx+");
    gROOT->LoadMacro("AliGMFSimpleTrackCuts.cxx+");

    // compile the jet finding classes
    gROOT->LoadMacro("AliGMFSimpleJetFinder.cxx+");

    TChain* myChain = new TChain("tree");
    myChain->Add(Form("/eos/user/r/rbertens/sandbox/mixed_events_nolimit_1000perfile/ME_%i.root", fileSuffix));


    // add more files if desired, e.g. per class

    TFile* of = new TFile("ME_jets.root", "RECREATE");

    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
 
    Int_t iEvents = reader->GetNumberOfEvents();
    cout << " Found ievents " << iEvents << endl;
    if(iEvents < 1) {
        delete reader;
        cout << " empty input file, aborting " << endl;
        return;
    }
   
    // create the jet finders
    AliGMFSimpleJetFinder* jetFinder[2];
    float radii[] = {.2, .4};
  
    // create track cuts
    AliGMFSimpleTrackCuts* trackCuts = new AliGMFSimpleTrackCuts();
    trackCuts->SetTrackMinPt(minConstPt);
    
    for(int i = 0; i < 2; i++) {
        jetFinder[i] = new AliGMFSimpleJetFinder();
        jetFinder[i]->SetJetResolution(radii[i]);
        jetFinder[i]->SetSplittingForTracksWithPtHigherThan(splitTracksFrom);
        jetFinder[i]->SetSplitTrackPt(splitThemIn);
        jetFinder[i]->SetRandomizeSplitTrackEtaPhi(randomize);   
        jetFinder[i]->SetLeadingHadronPt(leadingHadronPt);
        jetFinder[i]->SetLeadingHadronMaxPt(leadingHadronMaxPt);
        jetFinder[i]->SetTrackCuts(trackCuts);
        jetFinder[i]->SetRejectNHardestJets(0);
        jetFinder[i]->Initialize();
    }
    
    

    for (int i = 0 ; i < iEvents; i ++) {
        for(int j = 0; j < 2; j++) {
            jetFinder[j]->AnalyzeEvent(reader->GetEvent(i));
            cout <<"Event: " << i << "\r"; cout.flush();
        }
    }

    // write and clear memory
    for(int i = 0; i < 2; i++) {
        jetFinder[i]->Finalize(of);
        delete jetFinder[i];
    }

    delete reader;

}
