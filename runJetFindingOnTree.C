void runJetFindingOnTree(
        Int_t cenMin = 10,
        Int_t cenMax = 20,
        Int_t file = 0)
{

    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$PATH_TO_SOURCE -I$FASTJET/include");

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

    // add the desired numbers to a chain (not exception safe for now!)
    TChain* myChain = new TChain("tree");
    myChain->Add(Form("/lustre/medusa/rbertens/merge/%i.root", file));

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
   
    for(int i = 0; i < 1; i++) {
       jetFinder[i] = new AliGMFSimpleJetFinder();
       jetFinder[i]->SetJetResolution(radii[i]);
       // pass the event cuts to the jet finder
       jetFinder[i]->SetEventCuts(eventCuts);
       jetFinder[i]->Initialize();
    }

    for (int i = 0; i < iEvents; i ++) {
        for(int j = 0; j < 1; j++) {
            jetFinder[i]->AnalyzeEvent(reader->GetEvent(i));
        }
    }

    for (int i = 0; i < 1; i++) {
        jetFinder[i]->Finalize(Form("SE_jets_%i_%i_%i_R0%i", file, cenMin, cenMax, i+2));
        delete jetFinder[i];
    }
    
    delete reader;

}
