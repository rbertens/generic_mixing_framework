//#include "AliGMFEventReader.h"
//#include "AliGMFMixingManager.h"

void runEventMixer(
        Int_t minMult = 1000,
        Int_t maxMult = 1800,
        Float_t minVtx = -2,
        Float_t maxVtx = 2,
        Float_t minEp = 0,
        Float_t maxEp = 2,
        Float_t minCen = 0,
        Float_t maxCen = 10,
        Int_t year = 11)
{

    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)


    // $PATH_TO_SOURCE can refer to the path to the source
    // if left empty, it's a harmless qualifier

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated");
    // compile the classes
    gROOT->LoadMacro("AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("AliGMFEventContainer.cxx+");
    gROOT->LoadMacro("AliGMFEventReader.cxx+");
    gROOT->LoadMacro("AliGMFHistogramManager.cxx+");
    gROOT->LoadMacro("AliGMFMixingManager.cxx+");

    // define the input chain and create an event reader
    TChain* myChain = new TChain("tree");

    if(year == 10) {
        myChain->Add("$PATH_TO_DATA/137161.root");
        myChain->Add("$PATH_TO_DATA/137162.root");
        myChain->Add("$PATH_TO_DATA/137230.root");
        myChain->Add("$PATH_TO_DATA/137231.root");
        myChain->Add("$PATH_TO_DATA/137232.root");
        myChain->Add("$PATH_TO_DATA/137235.root");
        myChain->Add("$PATH_TO_DATA/137236.root");
        myChain->Add("$PATH_TO_DATA/137243.root");
        myChain->Add("$PATH_TO_DATA/137430.root");
        myChain->Add("$PATH_TO_DATA/137431.root");
        myChain->Add("$PATH_TO_DATA/137432.root");
        myChain->Add("$PATH_TO_DATA/137434.root");
        myChain->Add("$PATH_TO_DATA/137439.root");
        myChain->Add("$PATH_TO_DATA/137440.root");
        myChain->Add("$PATH_TO_DATA/137441.root");
        myChain->Add("$PATH_TO_DATA/137443.root");
        myChain->Add("$PATH_TO_DATA/137530.root");
        myChain->Add("$PATH_TO_DATA/137531.root");
        myChain->Add("$PATH_TO_DATA/137539.root");
        myChain->Add("$PATH_TO_DATA/137541.root");
        myChain->Add("$PATH_TO_DATA/137544.root");
        myChain->Add("$PATH_TO_DATA/137546.root");
        myChain->Add("$PATH_TO_DATA/137549.root");
        myChain->Add("$PATH_TO_DATA/137595.root");
        myChain->Add("$PATH_TO_DATA/137608.root");
        myChain->Add("$PATH_TO_DATA/137638.root");
        myChain->Add("$PATH_TO_DATA/137639.root");
        myChain->Add("$PATH_TO_DATA/137685.root");
        myChain->Add("$PATH_TO_DATA/137686.root");
        myChain->Add("$PATH_TO_DATA/137691.root");
        myChain->Add("$PATH_TO_DATA/137692.root");
        myChain->Add("$PATH_TO_DATA/137693.root");
        myChain->Add("$PATH_TO_DATA/137704.root");
        myChain->Add("$PATH_TO_DATA/137718.root");
        myChain->Add("$PATH_TO_DATA/137722.root");
        myChain->Add("$PATH_TO_DATA/137724.root");
        myChain->Add("$PATH_TO_DATA/137751.root");
        myChain->Add("$PATH_TO_DATA/137752.root");
        myChain->Add("$PATH_TO_DATA/137844.root");
        myChain->Add("$PATH_TO_DATA/137848.root");
        myChain->Add("$PATH_TO_DATA/138190.root");
        myChain->Add("$PATH_TO_DATA/138192.root");
        myChain->Add("$PATH_TO_DATA/138197.root");
        myChain->Add("$PATH_TO_DATA/138201.root");
        myChain->Add("$PATH_TO_DATA/138225.root");
        myChain->Add("$PATH_TO_DATA/138275.root");
        myChain->Add("$PATH_TO_DATA/138364.root");
        myChain->Add("$PATH_TO_DATA/138396.root");
        myChain->Add("$PATH_TO_DATA/138438.root");
        myChain->Add("$PATH_TO_DATA/138439.root");
        myChain->Add("$PATH_TO_DATA/138442.root");
        myChain->Add("$PATH_TO_DATA/138469.root");
        myChain->Add("$PATH_TO_DATA/138534.root");
        myChain->Add("$PATH_TO_DATA/138578.root");
        myChain->Add("$PATH_TO_DATA/138579.root");
        myChain->Add("$PATH_TO_DATA/138582.root");
        myChain->Add("$PATH_TO_DATA/138583.root");
        myChain->Add("$PATH_TO_DATA/138621.root");
        myChain->Add("$PATH_TO_DATA/138624.root");
        myChain->Add("$PATH_TO_DATA/138638.root");
        myChain->Add("$PATH_TO_DATA/138652.root");
        myChain->Add("$PATH_TO_DATA/138653.root");
        myChain->Add("$PATH_TO_DATA/138662.root");
        myChain->Add("$PATH_TO_DATA/138666.root");
        myChain->Add("$PATH_TO_DATA/138730.root");
        myChain->Add("$PATH_TO_DATA/138732.root");
        myChain->Add("$PATH_TO_DATA/138837.root");
        myChain->Add("$PATH_TO_DATA/138870.root");
        myChain->Add("$PATH_TO_DATA/138871.root");
        myChain->Add("$PATH_TO_DATA/138872.root");
        myChain->Add("$PATH_TO_DATA/139028.root");
        myChain->Add("$PATH_TO_DATA/139029.root");
        myChain->Add("$PATH_TO_DATA/139036.root");
        myChain->Add("$PATH_TO_DATA/139037.root");
        myChain->Add("$PATH_TO_DATA/139038.root");
        myChain->Add("$PATH_TO_DATA/139105.root");
        myChain->Add("$PATH_TO_DATA/139107.root");
        myChain->Add("$PATH_TO_DATA/139309.root");
        myChain->Add("$PATH_TO_DATA/139310.root");
        myChain->Add("$PATH_TO_DATA/139314.root");
        myChain->Add("$PATH_TO_DATA/139328.root");
        myChain->Add("$PATH_TO_DATA/139329.root");
        myChain->Add("$PATH_TO_DATA/139360.root");
        myChain->Add("$PATH_TO_DATA/139437.root");
        myChain->Add("$PATH_TO_DATA/139438.root");
        myChain->Add("$PATH_TO_DATA/139465.root");
        myChain->Add("$PATH_TO_DATA/139503.root");
        myChain->Add("$PATH_TO_DATA/139505.root");
        myChain->Add("$PATH_TO_DATA/139507.root");
        myChain->Add("$PATH_TO_DATA/139510.root");
    } else {
        myChain->Add("$PATH_TO_DATA/167902.root");
        myChain->Add("$PATH_TO_DATA/167903.root"); 
        myChain->Add("$PATH_TO_DATA/167915.root"); 
        myChain->Add("$PATH_TO_DATA/167920.root"); 
        myChain->Add("$PATH_TO_DATA/167987.root");
        myChain->Add("$PATH_TO_DATA/167988.root"); 
        myChain->Add("$PATH_TO_DATA/168066.root"); 
        myChain->Add("$PATH_TO_DATA/168068.root"); 
        myChain->Add("$PATH_TO_DATA/168069.root"); 
        myChain->Add("$PATH_TO_DATA/168076.root"); 
        myChain->Add("$PATH_TO_DATA/168104.root"); 
        myChain->Add("$PATH_TO_DATA/168107.root"); 
        myChain->Add("$PATH_TO_DATA/168108.root"); 
        myChain->Add("$PATH_TO_DATA/168115.root"); 
        myChain->Add("$PATH_TO_DATA/168212.root"); 
        myChain->Add("$PATH_TO_DATA/168310.root"); 
        myChain->Add("$PATH_TO_DATA/168311.root"); 
        myChain->Add("$PATH_TO_DATA/168322.root"); 
        myChain->Add("$PATH_TO_DATA/168325.root"); 
        myChain->Add("$PATH_TO_DATA/168341.root"); 
        myChain->Add("$PATH_TO_DATA/168342.root"); 
        myChain->Add("$PATH_TO_DATA/168361.root"); 
        myChain->Add("$PATH_TO_DATA/168362.root"); 
        myChain->Add("$PATH_TO_DATA/168458.root"); 
        myChain->Add("$PATH_TO_DATA/168460.root"); 
        myChain->Add("$PATH_TO_DATA/168461.root"); 
        myChain->Add("$PATH_TO_DATA/168464.root"); 
        myChain->Add("$PATH_TO_DATA/168467.root"); 
        myChain->Add("$PATH_TO_DATA/168511.root"); 
        myChain->Add("$PATH_TO_DATA/168512.root"); 
        myChain->Add("$PATH_TO_DATA/168777.root"); 
        myChain->Add("$PATH_TO_DATA/168826.root"); 
        myChain->Add("$PATH_TO_DATA/168984.root"); 
        myChain->Add("$PATH_TO_DATA/168988.root"); 
        myChain->Add("$PATH_TO_DATA/168992.root"); 
        myChain->Add("$PATH_TO_DATA/169035.root"); 
        myChain->Add("$PATH_TO_DATA/169091.root"); 
        myChain->Add("$PATH_TO_DATA/169094.root"); 
        myChain->Add("$PATH_TO_DATA/169138.root"); 
        myChain->Add("$PATH_TO_DATA/169143.root"); 
        myChain->Add("$PATH_TO_DATA/169144.root"); 
        myChain->Add("$PATH_TO_DATA/169145.root"); 
        myChain->Add("$PATH_TO_DATA/169148.root"); 
        myChain->Add("$PATH_TO_DATA/169156.root"); 
        myChain->Add("$PATH_TO_DATA/169160.root"); 
        myChain->Add("$PATH_TO_DATA/169167.root"); 
        myChain->Add("$PATH_TO_DATA/169238.root"); 
        myChain->Add("$PATH_TO_DATA/169411.root"); 
        myChain->Add("$PATH_TO_DATA/169415.root"); 
        myChain->Add("$PATH_TO_DATA/169417.root"); 
        myChain->Add("$PATH_TO_DATA/169835.root"); 
        myChain->Add("$PATH_TO_DATA/169837.root"); 
        myChain->Add("$PATH_TO_DATA/169838.root"); 
        myChain->Add("$PATH_TO_DATA/169846.root"); 
        myChain->Add("$PATH_TO_DATA/169855.root"); 
        myChain->Add("$PATH_TO_DATA/169858.root"); 
        myChain->Add("$PATH_TO_DATA/169859.root"); 
        myChain->Add("$PATH_TO_DATA/169923.root"); 
        myChain->Add("$PATH_TO_DATA/169956.root"); 
        myChain->Add("$PATH_TO_DATA/170027.root"); 
        myChain->Add("$PATH_TO_DATA/170036.root"); 
        myChain->Add("$PATH_TO_DATA/170081.root");
    }

    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    // create the mixer and connect the input event reader
    AliGMFMixingManager* mixer = new AliGMFMixingManager();
    mixer->SetEventReader(reader);

    // configure the mixer
    mixer->SetMultiplicityRange(minMult, maxMult);
    mixer->SetVertexRange(minVtx, maxVtx);
    mixer->SetEventPlaneRange(minEp, maxEp);
    mixer->SetCentralityRange(minCen, maxCen);
    mixer->SetMaxEventsPerFile(2500);
    //    mixer->SetMaxEvents(10000);
    mixer->DoQA();
    //    mixer->SetSplittingForTracksWithPtHigherThan(3.);
    //    mixer->SetSplitTrackPt(1.);


    // run the mixer
    mixer->DoPerChunkMixing();

    // cleanup
    delete mixer;
    delete myChain;
    delete reader;
    // sit back and enjoy the results
}
