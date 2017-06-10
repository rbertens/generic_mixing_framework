void runTTreeFilterOnGrid() {
    // author: Redmer Alexander Bertens, Utrecht University
    // rbertens@cern.ch (UTK, CERN, 2017)
    //
    // example which converts input data (in this case local aod's put into a chain)
    // to a tree which holds
    // - AliGMFTTreeHeader : event object
    // - AliGMFTTreeTrack : track objects
    // see source of these classes for more details

    // load libraries
    gSystem->Load("libANALYSISalice");

    // create and customize the alien plugin
    AliAnalysisAlien *alienHandler = new AliAnalysisAlien();
    alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
    alienHandler->SetAdditionalLibs("AliGMFEventCuts.cxx AliGMFEventCuts.h AliGMFTrackCuts.cxx AliGMFTrackCuts.h AliGMFTTreeHeader.cxx AliGMFTTreeTrack.cxx AliAnalysisTaskTTreeFilter.cxx AliGMFTTreeHeader.h AliGMFTTreeTrack.h AliAnalysisTaskTTreeFilter.h");
    alienHandler->SetAnalysisSource("AliGMFEventCuts.cxx AliGMFTrackCuts.cxx AliGMFTTreeHeader.cxx AliGMFTTreeTrack.cxx AliAnalysisTaskTTreeFilter.cxx");
    alienHandler->SetOverwriteMode();
    alienHandler->SetRunMode("terminate");
    alienHandler->SetNtestFiles(1);
    alienHandler->SetAPIVersion("V1.1x");
    alienHandler->SetAliPhysicsVersion("vAN-20170606-1");
    alienHandler->SetFileForTestMode("filelist.txt");
    alienHandler->SetGridDataDir("/alice/data/2010/LHC10h");
    alienHandler->SetDataPattern("*ESDs/pass2/AOD160/*AOD.root");
    alienHandler->SetRunPrefix("000");
    Int_t runs[] =  {139510, 139507, 139505, 139503, /*139465,*/ 139438, 139437, /*139360,*/ 139329, 139328, 139314, 139310, 139309, 139173, 139107, 139105, 139038, 139037, 139036, 139029, 139028, 138872, 138871, 138870, 138837, /*138732,*/ /*138730,*/ /*138666,*/ 138662, 138653, 138652, 138638, 138624, 138621, 138583, 138582, 138579, 138578, 138534, 138469, 138442, 138439, /*138438,*/ /*138396*/ 138364, 138225, 138201, 138197, 138192, 138190, 137848, 137844, 137752, 137751, 137724, 137722, 137718, 137704, 137693, 137692, 137691, 137686, 137685, 137639, 137638, 137608, 137595, 137549, /*137546,*/ 137544, 137541, 137539, 137531, 137530, 137443, 137441, 137440, 137439, 137434, 137432, 137431, 137430, 137243, 137236, 137235, 137232, 137231, /*137230,*/ 137162, 137161, /*137135*/ 138275}; // bad runs are commented out - 83 entries

    Int_t firstrun = 0;
    Int_t lastrun = 35;
    // add the runnnumbers to the handler
    for(int i = firstrun; i < lastrun; i++) alienHandler->AddRunNumber(runs[i]);

    alienHandler->SetDefaultOutputs();
    alienHandler->SetAnalysisMacro("TTreeFilterOnGrid.C");
    alienHandler->SetSplitMaxInputFileNumber(40);
    alienHandler->SetExecutable("runTTreeFilterOnGrid.sh");
    alienHandler->SetTTL(10000);
    alienHandler->SetInputFormat("xml-single");
    alienHandler->SetJDLName("runTTreeFilterOnGrid.jdl");
    alienHandler->SetPrice(1);
    alienHandler->SetSplitMode("se");

    alienHandler->SetOutputToRunNo(kTRUE);
    alienHandler->SetKeepLogs(kTRUE);
    alienHandler->SetMaxMergeStages(1);
    alienHandler->SetMergeViaJDL(kFALSE);

    // define the output folders
    alienHandler->SetGridWorkingDir(Form("filteredTTree_runs_%i-%i", runs[firstrun], runs[lastrun]));
    alienHandler->SetGridOutputDir(Form("filteredTTree_runs_%i-%i", runs[firstrun], runs[lastrun]));

    // create the analysis manager
    AliAnalysisManager* mgr = new AliAnalysisManager("MyManager");
    // connect the alien plugin to the manager
    mgr->SetGridHandler(alienHandler);

    AliVEventHandler* inputH = new AliAODInputHandler();
    // and connect it to the manager
    mgr->SetInputEventHandler(inputH);

    // compile the relevant classes
    gSystem->AddIncludePath("-Wno-deprecated");
    gSystem->AddIncludePath("-I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");

    gROOT->LoadMacro("AliGMFEventCuts.cxx+");
    gROOT->LoadMacro("AliGMFTrackCuts.cxx+");
    gROOT->LoadMacro("AliGMFTTreeHeader.cxx+");
    gROOT->LoadMacro("AliGMFTTreeTrack.cxx+");
    gROOT->LoadMacro("AliAnalysisTaskTTreeFilter.cxx+");

    // load the addtask
    gROOT->LoadMacro("add_task_macros/AddTaskTTreeFilter.C");

    // launch the task
    AddTaskTTreeFilter();

    // check if we can initialize the manager
    if(!mgr->InitAnalysis()) return;   
    // print the status of the manager to screen 
    mgr->PrintStatus();
    // print to screen how the analysis is progressing
    mgr->SetUseProgressBar(1, 25);
    // start the analysis locally, reading the events from the tchain
    mgr->StartAnalysis("grid");
}
