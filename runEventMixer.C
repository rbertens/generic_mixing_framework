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
    myChain->Add("merge/000137161.root");
    myChain->Add("merge/000137162.root");
    myChain->Add("merge/000137231.root");
    myChain->Add("merge/000137232.root");
    myChain->Add("merge/000137235.root");
    myChain->Add("merge/000137236.root");
    myChain->Add("merge/000137243.root");
    myChain->Add("merge/000137430.root");
    myChain->Add("merge/000137431.root");
    myChain->Add("merge/000137432.root");
    myChain->Add("merge/000137434.root");
    myChain->Add("merge/000137439.root");
    myChain->Add("merge/000137440.root");
    myChain->Add("merge/000137441.root");
    myChain->Add("merge/000137443.root");
    myChain->Add("merge/000137530.root");
    myChain->Add("merge/000137531.root");
    myChain->Add("merge/000137539.root");
    myChain->Add("merge/000137541.root");
    myChain->Add("merge/000137544.root");
    myChain->Add("merge/000137549.root");
    myChain->Add("merge/000137595.root");
    myChain->Add("merge/000137608.root");
    myChain->Add("merge/000137638.root");
    myChain->Add("merge/000137639.root");
    myChain->Add("merge/000137685.root");
    myChain->Add("merge/000137686.root");
    myChain->Add("merge/000137691.root");
    myChain->Add("merge/000137692.root");
    myChain->Add("merge/000137693.root");
    myChain->Add("merge/000137704.root");
    myChain->Add("merge/000137718.root");
    myChain->Add("merge/000137722.root");
    myChain->Add("merge/000137724.root");
    myChain->Add("merge/000137751.root");
    myChain->Add("merge/000137752.root");
    myChain->Add("merge/000137844.root");
    myChain->Add("merge/000137848.root");
    myChain->Add("merge/000138190.root");
    myChain->Add("merge/000138192.root");
    myChain->Add("merge/000138197.root");
    myChain->Add("merge/000138201.root");
    myChain->Add("merge/000138225.root");
//    myChain->Add("merge/000138275.root");
    myChain->Add("merge/000138364.root");
    myChain->Add("merge/000138439.root");
    myChain->Add("merge/000138442.root");
    myChain->Add("merge/000138469.root");
//    myChain->Add("merge/000138534.root");
    myChain->Add("merge/000138578.root");
    myChain->Add("merge/000138579.root");
    myChain->Add("merge/000138582.root");
    myChain->Add("merge/000138583.root");
    myChain->Add("merge/000138621.root");
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
//    myChain->Add("merge/000139038.root");
    myChain->Add("merge/000139105.root");
    myChain->Add("merge/000139107.root");
    myChain->Add("merge/000139173.root");
    myChain->Add("merge/000139309.root");
    myChain->Add("merge/000139310.root");
    myChain->Add("merge/000139314.root");
    myChain->Add("merge/000139328.root");
    myChain->Add("merge/000139329.root");
//    myChain->Add("merge/000139437.root");
    myChain->Add("merge/000139438.root");
    myChain->Add("merge/000139503.root");
    myChain->Add("merge/000139505.root");
    myChain->Add("merge/000139507.root");
    myChain->Add("merge/000139510.root");

    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    // create the mixer and connect the input event reader
    AliGMFMixingManager* mixer = new AliGMFMixingManager();
    mixer->SetEventReader(reader);

    // configure the mixer
    mixer->SetMultiplicityRange(200, 900);
    mixer->SetVertexRange(-5, 5);
    mixer->SetEventPlaneRange(-10, 10);
    mixer->SetCentralityRange(30, 50);
    mixer->DoQA();

    // run the mixer
    mixer->DoPerChunkMixing();

    // cleanup
    delete mixer;
    delete myChain;
    delete reader;
    // sit back and enjoy the results
}
