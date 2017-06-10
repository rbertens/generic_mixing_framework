 // author: redmer alexander bertens (rbertens@cern.ch)

#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TArrayI.h"

#include "AliLog.h"

#include "AliGMFMixingManager.h"
#include "AliGMFEventReader.h"
#include "AliGMFEventContainer.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFTTreeHeader.h"

ClassImp(AliGMFMixingManager);

//_____________________________________________________________________________
AliGMFMixingManager::AliGMFMixingManager() : TObject(),
    fMultiplicityMin(1), 
    fMultiplicityMax(-1),
    fVertexMin(1),
    fVertexMax(-1),
    fEventPlaneMin(1),
    fEventPlaneMax(-1),
    fTree(0x0),/*
    fEvent(0x0),
    fBufferedEvent(0x0), 
    fTrackArray(0x0),
    fOutputFile(0x0),
    fEventCache(0x0),
    fTrackCache(0x0),
    fEventReader(0x0),*/
    fGlobalBufferPosition(0)
{
  // default constructor
}
//_____________________________________________________________________________
Bool_t AliGMFMixingManager::Initialize() {

    fOutputFile = new TFile("myMixedEvents.root", "RECREATE");

    fTree = new TTree("tree", "Event data");
    fEvent = new AliGMFTTreeHeader();
    fTree->Branch("event", &fEvent);

    fTrackArray = new TClonesArray("AliGMFTTreeTrack", 1000);
    fTree->Bronch("track", "TClonesArray", &fTrackArray); 


}
//_____________________________________________________________________________
void AliGMFMixingManager::InitializeMixingCache() {
    // the mixing is performed 'per chunk' of M events, where M corresponds to the maximum multiplicity of
    // the mixed classes . in a first step, we go through the input chain, and check which events in the input
    // chain fulfill our mixing criteria
    // these are stored the first in the 'cache' as a bookkeeping utility

    if(fMultiplicityMax < 1) AliFatal(" Maximum multiplicity is too low \n");

    fEventCache = new TArrayI(fMultiplicityMax);
    fTrackCache = new TArrayI(fMultiplicityMax);
}
//_____________________________________________________________________________
Bool_t AliGMFMixingManager::FillMixingCache() {
    // go through the events in the reader until we've found M events that
    // can be used for mixing, caching the index numbers of the eligible events
    // we start at the buffer position
    
    AliGMFEventContainer* currentEvent(0x0);
    Int_t iCache(0);


    // first flush the current cache
    FlushMixingCache();

    while ((currentEvent = fEventReader->GetEvent(fGlobalBufferPosition))) {

        // the buffer position moves with each event 
        fGlobalBufferPosition++;
        if(IsSelected(currentEvent)) {
            // if an event is selected for mixing, cache its index
            fEventCache->AddAt(fGlobalBufferPosition, iCache);
            iCache++;
        }
        // if the cache is full, break the loop
        if(iCache + 1 == fMultiplicityMax) break;
    }
    // exit status is false when the end of the event buffer is reached, otherwise true
    if(!currentEvent) return kFALSE;
    return kTRUE;
}
//_____________________________________________________________________________
void AliGMFMixingManager::StageCachedEvent(Int_t i) {
    // retrieve the i-th good event and put it in the event buffer 
    fBufferedEvent = fEventReader->GetEvent(fEventCache->At(i));
}
//_____________________________________________________________________________
AliGMFTTreeTrack* AliGMFMixingManager::GetNextTrackFromEvent(Int_t i) {
    // get the next track from the i-th cached event

    // first stage the i-th event
    StageCachedEvent(i);

    // then retrieve the track buffer position for this track and get the track itself
    Int_t trackBufferPosition = fTrackCache->At(i);
    AliGMFTTreeTrack* track(fBufferedEvent->GetTrack(trackBufferPosition));

    // move the track buffer position for the i-th event
    fTrackCache->AddAt(trackBufferPosition+1, i);

    return track;
}
//_____________________________________________________________________________
void AliGMFMixingManager::FlushMixingCache() {
    // clean out the cache
    fTrackCache->Reset(0);
}
//_____________________________________________________________________________
Int_t AliGMFMixingManager::DoPerChunkMixing() {
    // the heart of this manager

    // 0) general initialization
    Initialize();
    
    // 1) initialize the mixing cache
    InitializeMixingCache();

    // 2) create new events, loop exits when end of true evens is reached
    //    mixing only starts when the buffer is 100% full
    while(FillMixingCache()) {
        CreateNewEvent();
    }

    // 3) write the tree to a file
    Finish();
}

//_____________________________________________________________________________ 
Bool_t AliGMFMixingManager::IsSelected(AliGMFEventContainer* event) {
    // check if this event meets the criteria for mixing
    if(!event) return kFALSE;
    if(event->GetZvtx() < fVertexMin || event->GetZvtx() > fVertexMax) return kFALSE;
    if(event->GetMultiplicity() > fMultiplicityMax || event->GetMultiplicity() < fMultiplicityMin) return kFALSE;
    if(event->GetEventPlane() > fEventPlaneMin || event->GetEventPlane() < fEventPlaneMin) return kFALSE;

    // if it doesn't fail, it passes :)
    return kTRUE;
}
//_____________________________________________________________________________
void AliGMFMixingManager::CreateNewEvent() 
{

    // store some faux info
    fEvent->SetZvtx(.5*(fVertexMin+fVertexMax));
    fEvent->SetEventPlane(.5*(fEventPlaneMin+fEventPlaneMax));
    
    // and then get the tracks
    AliGMFTTreeTrack* track(0x0);
    for(Int_t i(0); i < fMultiplicityMin; i++) {
        track = GetNextTrackFromEvent(i);
        AliGMFTTreeTrack* mixedTrack = new((*fTrackArray)[i]) AliGMFTTreeTrack();
        mixedTrack->SetPt(track->GetPt());
        mixedTrack->SetEta(track->GetEta());
        mixedTrack->SetPhi(track->GetPhi());
        mixedTrack->SetCharge(track->GetCharge());
    }
    // write the tree and perform cleanup
    PushToTTree();
}
//_____________________________________________________________________________
void AliGMFMixingManager::PushToTTree()
{
    // push info to tree and do cleanup for next iteration
    fTree->Fill();
    fTrackArray->Clear();
}
//_____________________________________________________________________________
void AliGMFMixingManager::Finish() {
    // write and close the files
    fTree->Write();
    fOutputFile->Close();
}
