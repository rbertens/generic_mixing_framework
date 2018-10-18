void runJetFindingOnMixedEvents(Int_t fileSuffix = 0,
        Float_t minConstPt = 0.,
        Float_t leadingHadronPt = 0,
        Float_t leadingHadronMaxPt = 1e9,
        Float_t splitTracksFrom = 1e9,
        Float_t splitThemIn = 0,
        Bool_t randomize = kTRUE,
        Int_t rejectHardJets = 0,
        Bool_t collinear = kFALSE)
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
    gROOT->LoadMacro("AliGMFHistogramManager.cxx++O");
    gROOT->LoadMacro("AliGMFTTreeHeader.cxx++O");
    gROOT->LoadMacro("AliGMFTTreeTrack.cxx++O");
    gROOT->LoadMacro("AliGMFEventContainer.cxx++O");
    gROOT->LoadMacro("AliGMFEventReader.cxx++O");
    gROOT->LoadMacro("AliGMFSimpleEventCuts.cxx++O");
    gROOT->LoadMacro("AliGMFSimpleTrackCuts.cxx++O");

    // compile the jet finding classes
    gROOT->LoadMacro("AliGMFSimpleJetFinder.cxx++O");

    TChain* myChain = new TChain("tree");
    myChain->Add(Form("ME_%i.root", fileSuffix));


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
    AliGMFSimpleJetFinder* jetFinder[3];
    float radii[] = {.2, .4, .6};
  
    // create track cuts
    AliGMFSimpleTrackCuts* trackCuts = new AliGMFSimpleTrackCuts();
    trackCuts->SetTrackMinPt(minConstPt);
    
    for(int i = 0; i < 3; i++) {
        jetFinder[i] = new AliGMFSimpleJetFinder();
        jetFinder[i]->SetJetResolution(radii[i]);
//        jetFinder[i]->SetJetResolutionBkg(radii[i]);
        jetFinder[i]->SetSplittingForTracksWithPtHigherThan(splitTracksFrom);
        jetFinder[i]->SetSplitTrackPt(splitThemIn);
        jetFinder[i]->SetRandomizeSplitTrackEtaPhi(randomize);   
        jetFinder[i]->SetPreserveSplitTrackPhi(randomize);
        jetFinder[i]->SetLeadingHadronPt(leadingHadronPt);
        jetFinder[i]->SetLeadingHadronMaxPt(leadingHadronMaxPt);
        jetFinder[i]->SetTrackCuts(trackCuts);
        jetFinder[i]->SetRejectNHardestJets(rejectHardJets);
        jetFinder[i]->SetIsME(kTRUE);
        jetFinder[i]->SetCollinearSplittingOverMEs(collinear);
        jetFinder[i]->SetDoRandomConeAnalysis(kTRUE);
        jetFinder[i]->Initialize();
    }
    
    
//    iEvents = 20;
    for (int i = 0 ; i < iEvents; i ++) {
        for(int j = 0; j < 3; j++) {
            jetFinder[j]->AnalyzeEvent(reader->GetEvent(i));
//            cout <<"Event: " << i << "\r"; cout.flush();
        }
    }
    // for fCollinearSplittingOverMEs, the first 5 events are used for caching
    // now we want to use them here
    if(collinear) {
        for (int i = 0 ; i < 5; i ++) {
            for(int j = 0; j < 3; j++) {
                jetFinder[j]->AnalyzeEvent(reader->GetEvent(i));
//                cout <<"Event: " << i << "\r"; cout.flush();
            }
        }
    }
   

    // write and clear memory
    for(int i = 0; i < 3; i++) {
        jetFinder[i]->Finalize(of);
        delete jetFinder[i];
    }

    delete reader;

}
