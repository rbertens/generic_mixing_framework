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
    gROOT->LoadMacro("tools/AliGMFHistogramManager.cxx+");
    gROOT->LoadMacro("filter/AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("filter/AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("reader/AliGMFEventContainer.cxx+");
    gROOT->LoadMacro("reader/AliGMFEventReader.cxx+");

    // compile the jet finding classes
    gROOT->LoadMacro("jetfinder/AliGMFDummyJetFinder.cxx+");

    TChain* myChain = new TChain("tree");
    myChain->Add("myFilteredTree.root");
    // add more files if desired, e.g. per class

    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    AliGMFDummyJetFinder* jetFinder = new AliGMFDummyJetFinder();

    jetFinder->Initialize();    // tbd pass enum on configuration

    // etc, this of course will go in the mixing class

    for (int i = 0 ; i < 5; i ++) {
        jetFinder->AnalyzeEvent(reader->GetEvent(i));
    }

    // write and clear memory
    jetFinder->Finalize();

    delete jetFinder;
    delete reader;

}
