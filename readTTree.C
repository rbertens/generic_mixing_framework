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
    gROOT->LoadMacro("../objects/AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("../objects/AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("../objects/AliGMFEventSimpleFromTTree.cxx+");

    // chain of files with filtered data    
    TChain* myChain = new TChain("tree");
    myChain->Add("/home/rbertens/Documents/CERN/ALICE_DATA/filtered/000167988.root");
    myChain->Add("/home/rbertens/Documents/CERN/ALICE_DATA/filtered/000168066.root");

    // create pointers for the branches
    AliGMFTTreeHeader* event = 0x0;
    myChain->SetBranchAddress("event", &event);
    TClonesArray* tracks = 0x0;
    myChain->SetBranchAddress("track", &tracks);
    // and an example track
    AliGMFTTreeTrack* firstTrack = 0x0;

    // set how many events you want to analyze
    Int_t maxEvents = 10000;
    // event loop
    printf(" > %i events in chain, processing %i of them < \n", myChain->GetEntries(), maxEvents);
    for(Int_t i = 0; i < myChain->GetEntries(); i++) {
        cout << " > Parsing event " << i << "\r"; cout.flush();
        myChain->GetEntry(i);

        // do the event magic - tbd what we want here
        AliGMFEventSimple eventMaker = AliGMFEventSimpleFromTTree(event, tracks, cutsPOI, cutsRP);
        maxEvents--;
        if(maxEvents < 1) break;
    }

}

