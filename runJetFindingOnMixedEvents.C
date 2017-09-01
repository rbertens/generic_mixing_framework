void runJetFindingOnMixedEvents(Int_t fileSuffix = 0)
{
    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$FASTJET/include -I$PATH_TO_SOURCE/");

    // load fastjet libraries
    gSystem->Load("libCGAL");
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
    myChain->Add(Form("/eos/user/r/rbertens/sandbox/mixed-events-ptcut_1_3/ME_%i.root", fileSuffix));


    // add more files if desired, e.g. per class

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
    AliGMFSimpleJetFinder* jetFinder[4];
    float radii[] = {.2, .3, .4, .5};
  
    for(int i = 0; i < 4; i++) {
        jetFinder[i] = new AliGMFSimpleJetFinder();
        jetFinder[i]->SetJetResolution(radii[i]);
        jetFinder[i]->Initialize();
    }
    
    

    for (int i = 0 ; i < iEvents; i ++) {
        for(int j = 0; j < 4; j++) {
            jetFinder[j]->AnalyzeEvent(reader->GetEvent(i));
        }
    }

    // write and clear memory
    for(int i = 0; i < 4; i++) {
        jetFinder[i]->Finalize(Form("myMixedJets_R0%i", i+2));
        delete jetFinder[i];
    }

    delete reader;

}
