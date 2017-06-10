// create mixed events from input chain
//
// Redmer Alexander Bertens, 2017 (UTK, CERN)

#include "fastjet/Selector.hh"
//#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/ClusterSequenceAreaBase.hh"


#include <iostream>
#include <vector>

#include "TH1F.h"
#include "TFile.h"
#include "TMath.h"

#include "AliGMFEventContainer.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFEventMixer.h"
#include "AliGMFHistogramManager.h"
#include "AliGMFTTreeHeader.h"
#include "AliGMFEventCuts.h"
#include "AliGMFTrackCuts.h"

ClassImp(AliGMFEventMixer)

using namespace std;

//_____________________________________________________________________________
AliGMFEventMixer::AliGMFEventMixer() : TObject(),
    fHistogramManager(0x0),
    fOutputFile(0x0),
    fEvent(0x0),
    fTrackArray(0x0)
{
    // default constructor
}
    
//_____________________________________________________________________________
Bool_t AliGMFEventMixer::Initialize() {
    // initialize
   
   // create the histograms (for now here)
   fHistogramManager = new AliGMFHistogramManager();


   // open file for writing, together with bronch we'll avoid a buffer overflow
   fOutputFile = TFile::Open("mixedEvents.root", "RECREATE");
   // create the ttree
   fTree = new TTree("tree", "Event data");

   // init the custom event 
   fEvent = new AliGMFTTreeHeader();
   // add the event branch to the tree
   fTree->Branch("event", &fEvent);
  
   // init the track tclonesarray
   fTrackArray = new TClonesArray("AliGMFTTreeTrack", 1000);
   // add clones array as branch via bronch
   fTree->Bronch("track", "TClonesArray", &fTrackArray);




   return kTRUE;

}
//_____________________________________________________________________________

Bool_t AliGMFEventMixer::AnalyzeEvent(AliGMFEventContainer* event) {
    // called for each event
   
    TObject* track(0x0); 
    for(Int_t i(0); i < event->GetNumberOfTracks(); i++) {
        track = event->GetTrack(i);
        if(track) {
                // MIX MIX MIX 
        }
    }


    // parse the tracks
    ParseTracks(event);

    // write the tree and perform cleanup
    PushToTTree();
    
    // jay !
    return kTRUE;


    fHistogramManager->Fill( ... );
}
//________________________________________________________________________
void AliAnalysisTaskTTreeFilter::ParseTracks(AliVEvent* event)
{
    // parse tracks
    for(Int_t i(0), acceptedTracks(0); i < event->GetNumberOfTracks(); i++) {
        // track loop
        AliVTrack* track(static_cast<AliVTrack*>(event->GetTrack(i)));
        if(!PassesCuts(track)) continue;

        // push accepted track to tree
        AliGMFTTreeTrack* acceptedTrack = new((*fTrackArray)[acceptedTracks]) AliGMFTTreeTrack();
        acceptedTracks++;
        // add info
        acceptedTrack->SetPt(track->Pt());
        acceptedTrack->SetEta(track->Eta());
        acceptedTrack->SetPhi(track->Phi());
        acceptedTrack->SetCharge(track->Charge());
    }
    return;
}
//________________________________________________________________________
void AliAnalysisTaskTTreeFilter::PushToTTree()
{
    // push info to tree and do cleanup for next iteration
    fTree->Fill();
    fTrackArray->Clear();
}

//_____________________________________________________________________________

Bool_t AliGMFEventMixer::Finalize() {

    fHistogramManager->StoreManager("mixedEvents.root");
    return kTRUE;

}
