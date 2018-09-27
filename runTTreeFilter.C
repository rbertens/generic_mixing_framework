void runTTreeFilter() {
    // author: Redmer Alexander Bertens
    // rbertens@cern.ch
    //
    // example which converts input data (in this case local aod's put into a chain)
    // to a tree which holds
    // - AliGMFTTreeHeader : event object
    // - AliGMFTTreeTrack : track objects
    // see source of these classes for more details

    // load libraries
    gSystem->Load("libANALYSISalice");

    // create the analysis manager
    AliAnalysisManager* mgr = new AliAnalysisManager("MyManager");

    // create a tchain which will point to an aod tree
    TChain* chain = new TChain("aodTree");
    // add a few files to the chain (change this so that your local files are added)
/*
    chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2010/LHC10h/000138638/ESDs/pass2/AOD160/0123/AliAOD.root");
    chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2010/LHC10h/000139507/ESDs/pass2/AOD160/0001/AliAOD.root");
    chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2010/LHC10h/000139507/ESDs/pass2/AOD160/0002/AliAOD.root");
    chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2010/LHC10h/000139507/ESDs/pass2/AOD160/0003/AliAOD.root");
    chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2010/LHC10h/000139507/ESDs/pass2/AOD160/0004/AliAOD.root");
    chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2010/LHC10h/000139510/ESDs/pass2/AOD160/0001/AliAOD.root");
    chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2010/LHC10h/000139510/ESDs/pass2/AOD160/0002/AliAOD.root");
    chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2010/LHC10h/000139510/ESDs/pass2/AOD160/0003/AliAOD.root");
*/



     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/001/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/002/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/003/AliAOD.root");/*
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/004/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/005/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/006/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/007/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/008/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/009/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/010/AliAOD.root");*/

    // create an input handler
    AliVEventHandler* inputH = new AliAODInputHandler();
    // and connect it to the manager
    mgr->SetInputEventHandler(inputH);

    // compile the relevant classes

    // include paths, necessary for compilation
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");

    gROOT->LoadMacro("AliGMFEventCuts.cxx+");
    gROOT->LoadMacro("AliGMFTrackCuts.cxx+");
    gROOT->LoadMacro("AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("AliGMFHistogramManager.cxx+");
    gROOT->LoadMacro("AliAnalysisTaskTTreeFilter.cxx+");

    // load the addtask
    gROOT->LoadMacro("add_task_macros/AddTaskTTreeFilter.C");

    // add mult selection task for run II
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
    AddTaskMultSelection(kFALSE);
    // launch the task
    AliAnalysisTaskTTreeFilter* task = AddTaskTTreeFilter();
    task->SetDoQA(kTRUE);

    // check if we can initialize the manager
    if(!mgr->InitAnalysis()) return;   
    // print the status of the manager to screen 
    mgr->PrintStatus();
    // print to screen how the analysis is progressing
    mgr->SetUseProgressBar(1, 25);
    // start the analysis locally, reading the events from the tchain
    mgr->StartAnalysis("local", chain);
}
