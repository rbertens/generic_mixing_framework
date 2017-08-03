void runEventMixer(
        Int_t minMult = 200,
        Int_t maxMult = 600,
        Float_t minVtx = -10,
        Float_t maxVtx = 10,
        Float_t minEp = -10,
        Float_t maxEp = 10,
        Float_t minCen = 40,
        Float_t maxCen = 50
        )
{

    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)


    // $PATH_TO_SOURCE can refer to the path to the source
    // if left empty, it's a harmless qualifier

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated -I$PATH_TO_SOURCE");

    // compile the classes
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFEventContainer.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFEventReader.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFHistogramManager.cxx+");
    gROOT->LoadMacro("$PATH_TO_SOURCE/AliGMFMixingManager.cxx+");

    // define the input chain and create an event reader
    TChain* myChain = new TChain("tree");
    myChain->Add("$PATH_TO_DATA/000137161.root");
    myChain->Add("$PATH_TO_DATA/000137162.root");
    myChain->Add("$PATH_TO_DATA/000137231.root");
    myChain->Add("$PATH_TO_DATA/000137232.root");
    myChain->Add("$PATH_TO_DATA/000137235.root");
    myChain->Add("$PATH_TO_DATA/000137236.root");
    myChain->Add("$PATH_TO_DATA/000137243.root");
    myChain->Add("$PATH_TO_DATA/000137430.root");
    myChain->Add("$PATH_TO_DATA/000137431.root");
    myChain->Add("$PATH_TO_DATA/000137432.root");
    myChain->Add("$PATH_TO_DATA/000137434.root");
    myChain->Add("$PATH_TO_DATA/000137439.root");
    myChain->Add("$PATH_TO_DATA/000137440.root");
    myChain->Add("$PATH_TO_DATA/000137441.root");
    myChain->Add("$PATH_TO_DATA/000137443.root");
    myChain->Add("$PATH_TO_DATA/000137530.root");
    myChain->Add("$PATH_TO_DATA/000137531.root");
    myChain->Add("$PATH_TO_DATA/000137539.root");
    myChain->Add("$PATH_TO_DATA/000137541.root");
    myChain->Add("$PATH_TO_DATA/000137544.root");
    myChain->Add("$PATH_TO_DATA/000137549.root");
    myChain->Add("$PATH_TO_DATA/000137595.root");
    myChain->Add("$PATH_TO_DATA/000137608.root");
    myChain->Add("$PATH_TO_DATA/000137638.root");
    myChain->Add("$PATH_TO_DATA/000137639.root");
    myChain->Add("$PATH_TO_DATA/000137685.root");
    myChain->Add("$PATH_TO_DATA/000137686.root");
    myChain->Add("$PATH_TO_DATA/000137691.root");
    myChain->Add("$PATH_TO_DATA/000137692.root");
    myChain->Add("$PATH_TO_DATA/000137693.root");
    myChain->Add("$PATH_TO_DATA/000137704.root");
    myChain->Add("$PATH_TO_DATA/000137718.root");
    myChain->Add("$PATH_TO_DATA/000137722.root");
    myChain->Add("$PATH_TO_DATA/000137724.root");
    myChain->Add("$PATH_TO_DATA/000137751.root");
    myChain->Add("$PATH_TO_DATA/000137752.root");
    myChain->Add("$PATH_TO_DATA/000137844.root");
    myChain->Add("$PATH_TO_DATA/000137848.root");
    myChain->Add("$PATH_TO_DATA/000138190.root");
    myChain->Add("$PATH_TO_DATA/000138192.root");
    myChain->Add("$PATH_TO_DATA/000138197.root");
    myChain->Add("$PATH_TO_DATA/000138201.root");
    myChain->Add("$PATH_TO_DATA/000138225.root");
//    myChain->Add("$PATH_TO_DATA/000138275.root");
    myChain->Add("$PATH_TO_DATA/000138364.root");
    myChain->Add("$PATH_TO_DATA/000138439.root");
    myChain->Add("$PATH_TO_DATA/000138442.root");
    myChain->Add("$PATH_TO_DATA/000138469.root");
//    myChain->Add("$PATH_TO_DATA/000138534.root");
    myChain->Add("$PATH_TO_DATA/000138578.root");
    myChain->Add("$PATH_TO_DATA/000138579.root");
    myChain->Add("$PATH_TO_DATA/000138582.root");
    myChain->Add("$PATH_TO_DATA/000138583.root");
    myChain->Add("$PATH_TO_DATA/000138621.root");
    myChain->Add("$PATH_TO_DATA/000138624.root");
    myChain->Add("$PATH_TO_DATA/000138638.root");
    myChain->Add("$PATH_TO_DATA/000138652.root");
    myChain->Add("$PATH_TO_DATA/000138653.root");
    myChain->Add("$PATH_TO_DATA/000138662.root");
    myChain->Add("$PATH_TO_DATA/000138837.root");
    myChain->Add("$PATH_TO_DATA/000138870.root");
    myChain->Add("$PATH_TO_DATA/000138871.root");
    myChain->Add("$PATH_TO_DATA/000138872.root");
    myChain->Add("$PATH_TO_DATA/000139028.root");
    myChain->Add("$PATH_TO_DATA/000139029.root");
    myChain->Add("$PATH_TO_DATA/000139036.root");
    myChain->Add("$PATH_TO_DATA/000139037.root");
//    myChain->Add("$PATH_TO_DATA/000139038.root");
    myChain->Add("$PATH_TO_DATA/000139105.root");
    myChain->Add("$PATH_TO_DATA/000139107.root");
    myChain->Add("$PATH_TO_DATA/000139173.root");
    myChain->Add("$PATH_TO_DATA/000139309.root");
    myChain->Add("$PATH_TO_DATA/000139310.root");
    myChain->Add("$PATH_TO_DATA/000139314.root");
    myChain->Add("$PATH_TO_DATA/000139328.root");
    myChain->Add("$PATH_TO_DATA/000139329.root");
//    myChain->Add("$PATH_TO_DATA/000139437.root");
    myChain->Add("$PATH_TO_DATA/000139438.root");
    myChain->Add("$PATH_TO_DATA/000139503.root");
    myChain->Add("$PATH_TO_DATA/000139505.root");
    myChain->Add("$PATH_TO_DATA/000139507.root");
    myChain->Add("$PATH_TO_DATA/000139510.root");

    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    // create the mixer and connect the input event reader
    AliGMFMixingManager* mixer = new AliGMFMixingManager();
    mixer->SetEventReader(reader);

    // configure the mixer
    mixer->SetMultiplicityRange(minMult, maxMult);
    mixer->SetVertexRange(minVtx, maxVtx);
    mixer->SetEventPlaneRange(minEp, maxEp);
    mixer->SetCentralityRange(minCen, maxCen);
    mixer->SetMaxEventsPerFile(100000);
    mixer->DoQA();

    // run the mixer
    mixer->DoPerChunkMixing();

    // cleanup
    delete mixer;
    delete myChain;
    delete reader;
    // sit back and enjoy the results
}
