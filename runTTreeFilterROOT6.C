#include "AliAnalysisTaskTTreeFilter.h"

void runTTreeFilterROOT6() {
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
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/001/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/002/AliAOD.root");
     chain->Add("/home/rbertens/Documents/CERN/data/ALICE_DATA/data/2015/LHC15o/000246757/pass1/AOD/003/AliAOD.root");

    // create an input handler
    AliVEventHandler* inputH = new AliAODInputHandler();
    // and connect it to the manager
    mgr->SetInputEventHandler(inputH);

    // compile the relevant classes

    // include paths, necessary for compilation
    gInterpreter->ProcessLine(".include $ROOTSYS/include");
    gInterpreter->ProcessLine(".include $ALICE_ROOT/include");

    gInterpreter->LoadMacro("AliGMFEventCuts.cxx++g");
    gInterpreter->LoadMacro("AliGMFTrackCuts.cxx++g");
    gInterpreter->LoadMacro("AliGMFTTreeHeader.cxx++g");
    gInterpreter->LoadMacro("AliGMFTTreeTrack.cxx++g");
    gInterpreter->LoadMacro("AliGMFHistogramManager.cxx++g");
    gInterpreter->LoadMacro("AliAnalysisTaskTTreeFilter.cxx++g");

    // load the addtask
    TMacro filter(gSystem->ExpandPathName("add_task_macros/AddTaskTTreeFilter.C"));
    AliAnalysisTaskTTreeFilter* task = reinterpret_cast<AliAnalysisTaskTTreeFilter*>(filter.Exec());

    // add mult selection task for run II
    TMacro multSelection(gSystem->ExpandPathName("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C"));
    AliMultSelectionTask* multSelectionTask = reinterpret_cast<AliMultSelectionTask*>(multSelection.Exec());
    // launch the task
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
