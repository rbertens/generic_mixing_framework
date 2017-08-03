void runJetFindingOnTree(
        Int_t cenMin = 10,
        Int_t cenMax = 20,
        Int_t fileMin = 0,
        Int_t fileMax = 10)
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

    // the filtered runnumbers
    Int_t runNumbers = {137161,137162,137231,137232,137235,137236,137243,137430,137431,137432,137434,137439,137440,137441,137443,137530,137531,137539,137541,137544,137549,137595,137608,137638,137639,137685,137686,137691,137692,137693,137704,137718,137722,137724,137751,137752,137844,137848,138190,138192,138197,138201,138225,138364,138439,138442,138469,138578,138579,138582,138583,138621,138624,138638,138652,138653,138662,138837,138870,138871,138872,139028,139029,139036,139037,139105,139107,139173,139309,139310,139314,139328,139329,139438,139503,139505,139507,139510};


    // add the desired numbers to a chain (not exception safe for now!)
    TChain* myChain = new TChain("tree");
    for(Int_t i = fileMin ; i < fileMax; i++) {
        myChain->Add(Form("$PATH_TO_DATA/000%i.root", runNumbers[i]));
    }

    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    cout << reader->GetNumberOfEvents() << " events available for analysis " << endl;

    // create the jet finder

    TStopwatch timer;
    timer.Start();

    Int_t iEvents = reader->GetNumberOfEvents();
    Float_t remainingTime = -1;

    // set max number of accepted events
    Int_t iMaxEvents = iEvents;

    // initialize the jet finder
    AliGMFSimpleJetFinder* jetFinder = new AliGMFSimpleJetFinder();
    jetFinder->Initialize();

    // create the event cuts
    AliGMFSimpleEventCuts* eventCuts = new AliGMFSimpleEventCuts();
    eventCuts->SetCentralityRange(cenMin, cenMax);

    // pass the event cuts to the jet finder
    jetFinder->SetEventCuts(eventCuts);



    for (int i = 0, j = 0 ; i < iEvents; i ++) {
        if(i==100) {
            remainingTime = timer.RealTime()/100.;
            cout << " - remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl;
        } else if (i > 0 && i%1000 == 0) cout << " - remaining time (min) approximately " << remainingTime*(iEvents-i)/60. << endl; 
        
    ==1421) {j++; continue;};
        if(!jetFinder->AnalyzeEvent(reader->GetEvent(i))) continue;
        j++;
        if(j > iMaxEvents) break;
        cout << " Processed event " << i << " of which accepted " << j << "\r"; cout.flush();
    }

    // write and clear memory
    jetFinder->Finalize("myJets");

    
    delete jetFinder;
    delete reader;

}
