void runTTreeFilterOnGrid() {
    // author: Redmer Alexander Bertens, Utrecht University
    // rbertens@cern.ch (UTK, CERN, 2017)
    //
    // example which converts input data (in this case local aod's put into a chain)
    // to a tree which holds
    // - AliGMFTTreeHeader : event object
    // - AliGMFTTreeTrack : track objects
    // see source of these classes for more details




    //_________________________________________________________________________


    // select range of runs to analyze (see runs[] for definition)
    Int_t firstrun = 0;
    Int_t lastrun = 75;// max 75 for 2015
    Int_t year = 2015;  // set to 
                                // 2010 for 2010
                                // 2011 for good 2011 runs
                                // 20111 for irocc13 issue runs
                                // 20112 for orocc08 issue runs
    Bool_t terminate =           kTRUE; // run in terminate mode
    Bool_t downloadoutput =      kFALSE;// get the final output files


    //_________________________________________________________________________



    if(downloadoutput) terminate = kFALSE;

    // load libraries
    gSystem->Load("libANALYSISalice");

    // create and customize the alien plugin
    AliAnalysisAlien *alienHandler = new AliAnalysisAlien();
    alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
    alienHandler->SetAdditionalLibs("AliGMFEventCuts.cxx AliGMFEventCuts.h AliGMFTrackCuts.cxx AliGMFTrackCuts.h AliGMFTTreeHeader.cxx AliGMFTTreeTrack.cxx AliGMFHistogramManager.cxx AliAnalysisTaskTTreeFilter.cxx AliGMFTTreeHeader.h AliGMFTTreeTrack.h AliGMFHistogramManager.h AliAnalysisTaskTTreeFilter.h");
    alienHandler->SetAnalysisSource("AliGMFEventCuts.cxx AliGMFTrackCuts.cxx AliGMFTTreeHeader.cxx AliGMFTTreeTrack.cxx AliGMFHistogramManager.cxx AliAnalysisTaskTTreeFilter.cxx");
    alienHandler->SetOverwriteMode();
    if(!terminate) alienHandler->SetRunMode("full");
    else alienHandler->SetRunMode("terminate");
    alienHandler->SetNtestFiles(10);
    alienHandler->SetAPIVersion("V1.1x");
    alienHandler->SetAliPhysicsVersion("vAN-20180901-1");
    alienHandler->SetFileForTestMode("filelist.txt");

    if(year == 2010) {
        // settings for 10h
        alienHandler->SetGridDataDir("/alice/data/2010/LHC10h");
        alienHandler->SetDataPattern("*ESDs/pass2/AOD160/*AOD.root");
    } else if(year == 2011 || year == 20111 || year == 20112) {
        // settings for 11h
        alienHandler->SetGridDataDir("/alice/data/2011/LHC11h_2");
        alienHandler->SetDataPattern("*ESDs/pass2/AOD145/*AOD.root");
    } else if(year == 2015) {
        alienHandler->SetGridDataDir("/alice/data/2015/LHC15o/");
        alienHandler->SetDataPattern("pass1/AOD/*/AliAOD.root");
    }

    alienHandler->SetRunPrefix("000");

    // good 10h runs
    Int_t runs[] =  {139510, 139507, 139505, 139503, 139465, 139438, 139437, 139360, 139329, 139328, 139314, 139310, 139309, 139173, 139107, 139105, 139038, 139037, 139036, 139029, 139028, 138872, 138871, 138870, 138837, 138732, 138730, 138666, 138662, 138653, 138652, 138638, 138624, 138621, 138583, 138582, 138579, 138578, 138534, 138469, 138442, 138439, 138438, 138396, 138364, 138275, 138225, 138201, 138197, 138192, 138190, 137848, 137844, 137752, 137751, 137724, 137722, 137718, 137704, 137693, 137692, 137691, 137686, 137685, 137639, 137638, 137608, 137595, 137549, 137546, 137544, 137541, 137539, 137531, 137530, 137443, 137441, 137440, 137439, 137434, 137432, 137431, 137430, /*137366,*/ 137243, 137236, 137235, 137232, 137231, 137230, 137162, 137161 };

    // good tpc runs 11h - 63
    Int_t runs11hGoodTPC[] = { 167902,167903, 167915, 167920, 167987, 167988, 168066, 168068, 168069, 168076, 168104, 168107, 168108, 168115, 168212, 168310, 168311, 168322, 168325, 168341, 168342, 168361, 168362, 168458, 168460, 168461, 168464, 168467, 168511, 168512, 168777, 168826, 168984, 168988, 168992, 169035, 169091, 169094, 169138, 169143, 169144, 169145, 169148, 169156, 169160, 169167, 169238, 169411, 169415, 169417, 169835, 169837, 169838, 169846, 169855, 169858, 169859, 169923, 169956, 170027, 170036, 170081};

    // semi good tpc runs 11h - 26
    Int_t runs11hSemiGoodTPCIrocC13[] = {169975, 169981, 170038, 170040, 170083, 170084, 170085, 170088, 170089, 170091, 170152, 170155, 170159, 170163, 170193, 170195, 170203, 170204, 170228, 170230, 170268, 170269, 170270, 170306, 170308, 170309};
    // and the last batch - 24
    Int_t runs11hSemiGoodTPCOrocC08[] = {169040, 169044, 169045, 169099, 169418, 169419, 169420, 169475, 169498, 169504, 169506, 169512, 169515, 169550, 169553, 169554, 169555, 169557, 169584, 169586, 169587, 169588, 169590, 169591};

    Int_t runs15o[] = {246994, 246991, 246989, 246984, 246982, 246948, 246945, 246928, 246851, 246847, 246846, 246845, 246844, 246810, 246809, 246808, 246807, 246805, 246804, 246766, 246765, 246763, 246760, 246759, 246758, 246757, 246751, 246750, 246495, 246493, 246488, 246487, 246434, 246431, 246424, 246276, 246275, 246272, 246271, 246225, 246222, 246217, 246185, 246182, 246181, 246180, 246178, 246153, 246152, 246151, 246115, 246113, 246089, 246087, 246053, 246052, 246049, 246048, 246042, 246037, 246036, 246012, 246003, 246001, 245954, 245952, 245949, 245923, 245833, 245831, 245829, 245705, 245702, 245692, 245683}; 


    // add the runnnumbers to the handler
    if(year == 2010) for(int i = firstrun; i < lastrun; i++) alienHandler->AddRunNumber(runs[i]);
    else if (year == 2011) for(int i = firstrun; i < lastrun; i++) alienHandler->AddRunNumber(runs11hGoodTPC[i]);
    else if (year == 20111) for(int i = firstrun; i < lastrun; i++) alienHandler->AddRunNumber(runs11hSemiGoodTPCIrocC13[i]);
    else if (year == 20112) for(int i = firstrun; i < lastrun; i++) alienHandler->AddRunNumber(runs11hSemiGoodTPCOrocC08[i]);
    else if (year == 2015) for(int i = firstrun; i < lastrun; i++) alienHandler->AddRunNumber(runs15o[i]);

    alienHandler->SetDefaultOutputs();
    alienHandler->SetAnalysisMacro("TTreeFilterOnGrid.C");
    alienHandler->SetSplitMaxInputFileNumber(150);
    alienHandler->SetExecutable("runTTreeFilterOnGrid.sh");
    alienHandler->SetTTL(10000);
    alienHandler->SetInputFormat("xml-single");
    alienHandler->SetJDLName("runTTreeFilterOnGrid.jdl");
    alienHandler->SetPrice(1);
    alienHandler->SetSplitMode("se");

    alienHandler->SetOutputToRunNo(kTRUE);
    alienHandler->SetKeepLogs(kTRUE);
    alienHandler->SetMaxMergeStages(1);
    if(!downloadoutput) alienHandler->SetMergeViaJDL(kTRUE);
    else alienHandler->SetMergeViaJDL(kFALSE);

    // define the output folders
    alienHandler->SetGridWorkingDir("filteredTTree");
    alienHandler->SetGridOutputDir("filteredTTree");

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

    gROOT->LoadMacro("AliGMFEventCuts.cxx++O");
    gROOT->LoadMacro("AliGMFTrackCuts.cxx++O");
    gROOT->LoadMacro("AliGMFTTreeHeader.cxx++O");
    gROOT->LoadMacro("AliGMFTTreeTrack.cxx++O");
    gROOT->LoadMacro("AliGMFHistogramManager.cxx++O");
    gROOT->LoadMacro("AliAnalysisTaskTTreeFilter.cxx++O");


    // add mult selection task for run II
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
    AddTaskMultSelection(kFALSE);

    // load the addtask
    gROOT->LoadMacro("add_task_macros/AddTaskTTreeFilter.C");

    // launch the task
    AliAnalysisTaskTTreeFilter* task;
    if(year == 2010) task = AddTaskTTreeFilter();
    else {
        task = AddTaskTTreeFilter(
                TString("myFilteredTTree2015.root"),
                AliVEvent::kINT7);
        AliGMFEventCuts* eventcuts = task->GetEventCuts();
        eventcuts->Set2010PileUpRejection(kFALSE);
    }



    // check if we can initialize the manager
    if(!mgr->InitAnalysis()) return;   

    // print the status of the manager to screen 
    mgr->PrintStatus();
    mgr->StartAnalysis("grid");
}
