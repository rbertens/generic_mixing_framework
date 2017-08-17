void runJetFindingOnMixedEvents(
        Int_t firstFile = 0,
        Int_t lastFile = 1)
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
    for(Int_t i = firstFile; i < lastFile+1; i++) {
        myChain->Add(Form("/nics/c/home/rbertens/lustre/mixed-events/m_700_900_4_10_.785_1.6_40_50/myMixedEvents.root"));
//        cout << Form("/nics/c/home/rbertens/lustre/mixed-events/ME_%i.root", i) << endl;
    }


    // add more files if desired, e.g. per class

    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    
    // create the jet finders
    AliGMFSimpleJetFinder* jetFinder[4];
    float radii[] = {.2, .3, .4, .5};
  
    for(int i = 0; i < 4; i++) {
        jetFinder[i] = new AliGMFSimpleJetFinder();
        jetFinder[i]->SetJetResolution(radii[i]);
        jetFinder[i]->Initialize();
    }
    
    
    Int_t iEvents = reader->GetNumberOfEvents();
    cout << " ievents " << iEvents << endl;

    for (int i = 0 ; i < iEvents; i ++) {
        for(int j = 0; j < 4; j++) {
            jetFinder[j]->AnalyzeEvent(reader->GetEvent(i));
        }
    }

    // write and clear memory
    for(int i = 0; i < 4; i++) {
        jetFinder[i]->Finalize(Form("myMixedJets_R0%i_fromFiles%i_%i", i+2, firstFile, lastFile));
        delete jetFinder[i];
    }

    delete reader;

}
