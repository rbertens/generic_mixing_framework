void runEventMixer()
{

    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
 
    // compile the encapsulated classes
    gROOT->LoadMacro("AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("AliGMFEventContainer.cxx+");
    gROOT->LoadMacro("AliGMFEventReader.cxx+");
    gROOT->LoadMacro("AliGMFHistogramManager.cxx+");
    gROOT->LoadMacro("AliGMFMixingManager.cxx+");

    // define the input chain and create an event reader
    TChain* myChain = new TChain("tree");
    myChain->Add("merge/000138624.root");
    myChain->Add("merge/000138638.root");
    myChain->Add("merge/000138652.root");
    myChain->Add("merge/000138653.root");
    myChain->Add("merge/000138662.root");
    myChain->Add("merge/000138837.root");
    myChain->Add("merge/000138870.root");
    myChain->Add("merge/000138871.root");
    myChain->Add("merge/000138872.root");
    myChain->Add("merge/000139028.root");
    myChain->Add("merge/000139029.root");
    myChain->Add("merge/000139036.root");
    myChain->Add("merge/000139037.root");
    myChain->Add("merge/000139038.root");
    
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    // create the mixer and connect the input event reader
    AliGMFMixingManager* mixer = new AliGMFMixingManager();
    mixer->SetEventReader(reader);

    // configure the mixer
    mixer->SetMultiplicityRange(50, 100);
    mixer->SetVertexRange(-5, 5);
    mixer->SetEventPlaneRange(-10, 10);
    mixer->DoQA();

    // run the mixer
    mixer->DoPerChunkMixing();

    // cleanup
    delete mixer;
    delete myChain;
    delete reader;
    // sit back and enjoy the results
}
