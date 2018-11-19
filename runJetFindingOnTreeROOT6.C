
R__ADD_INCLUDE_PATH("$FASTJET/include")

#include "AliGMFEventReader.h"
//#include "AliGMFSimpleJetFinder.h"
#include "AliGMFSimpleEventCuts.h"
#include "AliGMFSimpleTrackCuts.h"


R__LOAD_LIBRARY(libfastjet);
R__LOAD_LIBRARY(libsiscone);
R__LOAD_LIBRARY(libsiscone_spherical);
R__LOAD_LIBRARY(libfastjetplugins);
R__LOAD_LIBRARY(libfastjettools);
R__LOAD_LIBRARY(libfastjetcontribfragile);

R__LOAD_LIBRARY(AliGMFHistogramManager_cxx.so);
R__LOAD_LIBRARY(AliGMFTTreeHeader_cxx.so);
R__LOAD_LIBRARY(AliGMFTTreeTrack_cxx.so);
R__LOAD_LIBRARY(AliGMFEventContainer_cxx.so);
R__LOAD_LIBRARY(AliGMFEventReader_cxx.so);
R__LOAD_LIBRARY(AliGMFSimpleTrackCuts_cxx.so);
R__LOAD_LIBRARY(AliGMFSimpleEventCuts_cxx.so);
R__LOAD_LIBRARY(AliGMFSimpleJetFinder_cxx.so);


void runJetFindingOnTreeROOT6(
        Int_t cenMin = 0,
        Int_t cenMax = 10,
        Int_t file = 246945,
        Int_t rejectNJ = 2,
        Float_t minConstPt = 0.,
        Float_t leadingHadronPt = 0,
        Float_t leadingHadronMaxPt = 1e9,
        Float_t splitTracksFrom = 1e9,
        Float_t splitThemIn = 0,
        Bool_t randomize = kFALSE,
        Bool_t randomizeEtaPhi = kFALSE)
{

    // example macro to read data from a ttree and perform simple analysis
    // author: Redmer Alexander Bertens (rbertens@cern.ch)

    //    gSystem->AddIncludePath("-Wno-deprecated");
    //    gSystem->AddIncludePath("-I$FASTJET/include");


    // add the desired numbers to a chain (not exception safe for now!)
    TChain* myChain = new TChain("tree");
    myChain->Add(Form("../%i.root", file));
    //    myChain->Add("merge/137844.root"); 
    // initialize the reader and jet finder
    AliGMFEventReader* reader = new AliGMFEventReader(myChain);
    cout << reader->GetNumberOfEvents() << " events available for analysis " << endl;


    TFile* of = new TFile(Form("SE_jets_%i_%i.root", cenMin, cenMax), "RECREATE");

    Int_t iEvents = reader->GetNumberOfEvents();

    // initialize the jet finder
    AliGMFSimpleJetFinder* jetFinder[3];
    float radii[] = {.2, .4, .6};
    // create the event cuts
    AliGMFSimpleEventCuts* eventCuts = new AliGMFSimpleEventCuts();
    eventCuts->SetCentralityRange(cenMin, cenMax);
    AliGMFSimpleTrackCuts* trackCuts = new AliGMFSimpleTrackCuts();
    trackCuts->SetTrackMinPt(minConstPt);

    for(int i = 0; i < 3; i++) {
        jetFinder[i] = new AliGMFSimpleJetFinder();
        jetFinder[i]->SetJetResolution(radii[i]);
        jetFinder[i]->SetJetResolutionBkg(radii[i]);
        jetFinder[i]->SetSplittingForTracksWithPtHigherThan(splitTracksFrom);
        jetFinder[i]->SetSplitTrackPt(splitThemIn);
        jetFinder[i]->SetRandomizeSplitTrackEtaPhi(randomize);
        jetFinder[i]->SetLeadingHadronPt(leadingHadronPt);
        jetFinder[i]->SetLeadingHadronMaxPt(leadingHadronMaxPt);
        // pass the event cuts to the jet finder
        jetFinder[i]->SetEventCuts(eventCuts);
        jetFinder[i]->SetTrackCuts(trackCuts);
        jetFinder[i]->SetRejectNHardestJets(rejectNJ);
        jetFinder[i]->SetDoRandomConeAnalysis(kTRUE);
        jetFinder[i]->Initialize();
    }

    for (int i = 0; i < iEvents; i ++) {
        for(int j = 0; j < 3; j++) {
            jetFinder[j]->AnalyzeEvent(reader->GetEvent(i));
            //            cout <<"Event: " << i << "\r"; cout.flush();
        }
    }

    for (int i = 0; i < 3; i++) {
        jetFinder[i]->Finalize(of);
        delete jetFinder[i];
    }

    delete reader;

}
