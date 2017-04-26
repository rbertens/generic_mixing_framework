 // author: redmer alexander bertens (rbertens@cern.ch)

#include "AliAnalysisTaskTTreeFilter.h"

// ROOT includes
#include <TTree.h>
#include <TMath.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TString.h>

// AliRoot includes
#include <AliAnalysisManager.h>
#include <AliInputEventHandler.h>
#include <AliLog.h>
#include <AliVEvent.h>
#include <AliVVertex.h>
#include <AliCentrality.h>
#include <AliVTrack.h>

// local includes
#include "AliGMFTTreeHeader.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFEventCuts.h"
#include "AliGMFTrackCuts.h"


ClassImp(AliAnalysisTaskTTreeFilter)

//_____________________________________________________________________________
AliAnalysisTaskTTreeFilter::AliAnalysisTaskTTreeFilter():
    AliAnalysisTaskSE(),
    fEvent(0x0),
    fTrackArray(0x0),
    fEventCuts(0x0),
    fTrackCuts(0x0)
{
    // default constructor for root I/O
}
//______________________________________________________________________________
AliAnalysisTaskTTreeFilter::AliAnalysisTaskTTreeFilter(const char *name):
    AliAnalysisTaskSE(name),
    fEvent(0x0),
    fTrackArray(0x0),
    fEventCuts(0x0),
    fTrackCuts(0x0)
{
    // constructor
    DefineOutput(1, TTree::Class());
}
//_____________________________________________________________________________
AliAnalysisTaskTTreeFilter::~AliAnalysisTaskTTreeFilter()
{
    // destructor
    delete fTree;
    delete fEventCuts;
    delete fTrackCuts;
}
//______________________________________________________________________________
void AliAnalysisTaskTTreeFilter::UserCreateOutputObjects()
{ 
    // check for manager
    AliAnalysisManager *man=AliAnalysisManager::GetAnalysisManager();
    AliInputEventHandler *inputHandler=dynamic_cast<AliInputEventHandler*>(man->GetInputEventHandler());
    if (!inputHandler) AliFatal(" > no input detected - aborting <");

    // open file at slot 1 for large output to avoid buffer overflow
    AliAnalysisTask::OpenFile(1);

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

    // Post output data.
    PostData(1, fTree);
}
//______________________________________________________________________________
void AliAnalysisTaskTTreeFilter::UserExec(Option_t *) 
{
    // parse accepted input event
    if(ParseEvent(dynamic_cast<AliVEvent*>(InputEvent()))) PostData(1, fTree);
    else return;
}

//________________________________________________________________________
Bool_t AliAnalysisTaskTTreeFilter::ParseEvent(AliVEvent* event) 
{
    // parse the input event
    if(!PassesCuts(event)) return kFALSE;

    // store some event info
    fEvent->SetZvtx(event->GetPrimaryVertex()->GetZ());
    
    // just some dummy value
    fEvent->SetEventPlane(2.);
    // parse the tracks
    ParseTracks(event);

    // write the tree and perform cleanup
    PushToTTree();
    
    // jay !
    return kTRUE;
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
//________________________________________________________________________
Bool_t AliAnalysisTaskTTreeFilter::PassesCuts(AliVEvent* event)
{
    // check the event cuts
    return fEventCuts->IsSelected(event);
}
//________________________________________________________________________
Bool_t AliAnalysisTaskTTreeFilter::PassesCuts(AliVTrack* track)
{
    // track cuts would go here
    return fTrackCuts->IsSelected(track);
}
//________________________________________________________________________
void AliAnalysisTaskTTreeFilter::Terminate(Option_t *)
{ 
    // terminate
}
