void readTTree()
{

    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
 
    // load libraries
    gSystem->Load("libPWGflowBase");

    // comile the encapsulated classes
    gROOT->LoadMacro("../filter/AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("../filter/AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("../filter/AliGMFEventSimpleFromTTree.cxx+");
    gROOT->LoadMacro("../reader/AliGMFEventContainer.cxx+");
    gROOT->LoadMacro("../reader/AliGMFEventReader.cxx+");

    TChain* myChain = new TChain("tree");
    myChain->Add("../myFilteredTree.root");
    // add more files if desired, e.g. per class

    // initialize the reader and manipulate the events
    AliGMFEventReader reader = new AliGMGEventReader();
    reader->SetInputChain(myChain);

    // etc, this of course will go in the mixing class
    AliGMFEventReader->GetEvent(i);




}

