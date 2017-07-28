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
    gROOT->LoadMacro("AliGMFHistogramManager.cxx+");
    gROOT->LoadMacro("AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("AliGMFEventContainer.cxx+");
    gROOT->LoadMacro("AliGMFEventReader.cxx+");
    gROOT->LoadMacro("AliGMFSimpleEventCuts.cxx+");

    // compile the jet finding classes
    gROOT->LoadMacro("AliGMFSimpleJetFinder.cxx+");

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





    // add more files if desired, e.g. per class

    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    cout << reader->GetNumberOfEvents() << " events available for analysis " << endl;

    // create the jet finder

    TStopwatch timer;
    timer.Start();

    Int_t iEvents = reader->GetNumberOfEvents();
    Float_t remainingTime = -1;

    // set max number of accepted events
    Int_t iMaxEvents = 1420;

    for(int a = 4; a < 5; a++) {
        AliGMFSimpleJetFinder* jetFinder = new AliGMFSimpleJetFinder();
        jetFinder->Initialize();    // tbd pass enum on configuratioin



        // create the event cuts
        AliGMFSimpleEventCuts* eventCuts = new AliGMFSimpleEventCuts();
        eventCuts->SetMultiplicityRange(0,10000);
        eventCuts->SetVertexRange(-5, 5);
        eventCuts->SetEventPlaneRange(-10, 10);


        eventCuts->SetCentralityRange(a*10, (a+1)*10);

        // pass the event cuts to the jet finder
        jetFinder->SetEventCuts(eventCuts);



        for (int i = 0, j = 0 ; i < iEvents; i ++) {
            if(i==100) {
                remainingTime = timer.RealTime()/100.;
                cout << " - remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl;
            } else if (i > 0 && i%1000 == 0) cout << " - remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl; 
            
    if(j==1421) {j++; continue;};
            if(!jetFinder->AnalyzeEvent(reader->GetEvent(i))) continue;
            j++;
            if(j > iMaxEvents) break;
            cout << " Processed event " << i << " of which accepted " << j << "\r"; cout.flush();
        }

        // write and clear memory
        jetFinder->Finalize(Form("myJets_%i_%i", a, a+1));

    }
    delete jetFinder;
    delete reader;

}
