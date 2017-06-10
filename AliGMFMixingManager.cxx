 // author: redmer alexander bertens (rbertnens@cern.ch)

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

AliGMFMixingManager::AliGMFTrackCuts() : TObject(),
    fMultiplicityMin(1), 
    fMultipicityMax(-1),
    fVertexMin(1),
    fVertexMax(-1)    
    fEventPlaneMin(1),
    fEventPlaneMax(-1),
    fTree(0x0),
    fEvent(0x0),
    fBufferedEvent(0x0),  
    fTrackArray(0x0),
    fOutputFile(0x0),
    fEventCache(0x0),
    fTrackCache(0x0),
    fEventReader(0x0),
    fGlobalBufferPosition(0)
{
  // default constructor
}
//_____________________________________________________________________________
Bool_t Initialize() {

    fOutputFile = new TFile("myMixedEvents.root", "RECREATE");

    fTree = new TTree("tree", "Event data");
    fEvent = new AliGMFTTreeHeader();
    fTree->Branch("event", &fEvent);

    fTrackArray = new TClonesArray("AliGMFTTreeTrack", 1000);
    fTree->Bronch("track", "TClonesArray", &fTrackArray); 


}
//_____________________________________________________________________________
void InitializeMixingCache() {
    // the mixing is performed 'per chunk' of M events, where M corresponds to the maximum multiplicity of
    // the mixed classes . in a first step, we go through the input chain, and check which events in the input
    // chain fulfill our mixing criteria
    // these are stored the first in the 'cache' as a bookkeeping utility

    if(fMultiplicityMax < 1) AliFatal(" Maximum multiplicity is too low \n");

    fEventCache = new TArrayI(fMultiplicityMax);
    fTrackCache = new TArrayI(fMultiplicityMax);
}
//_____________________________________________________________________________
Bool_t FillMixingCache() {
    // go through the events in the reader until we've found M events that
    // can be used for mixing, caching the index numbers of the eligible events
    // we start at the buffer position
    
    AliGMFEventContainer* currentEvent(0x0);
    Int_t iCache(0);


    // first flush the current cache
    FlushMixingCache();

    while ((currentEvent = fReader->GetEvent(fGlobalBufferPosition))) {

        // the buffer position moves with each event 
        fGlobalBufferPosition++;
        if(IsSelected(currentEvent)) {
            // if an event is selected for mixing, cache its index
            fEventCache->AddAt(iCache, fGlobalBufferPosition);
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
    fBufferedEvent = fReader->GetEvent(fEventCache->At(i));
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
void FlushMixingCache() {
    // clean out the cache
    fTrackCache->Reset(0);
}
//_____________________________________________________________________________
Int_t DoPerChunkMixing() {
    // the heart of this manager

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
    if(event->GetMultiplicity() > fMultipicityMax || event->GetMultiplicity() < fMultiplicityMin) return kFALSE;
    if(event->GetEventPlane() > fEventPlaneMin || event->GetEventPlane() < fEventPlaneMin()) return kFALSE;

    // if it doesn't fail, it passes :)
    return kTRUE;
}
//_____________________________________________________________________________
Bool_t AliGMFMixingManager::CreateNewEvent() 
{

    // store some faux info
    fEvent->SetZvtx(.5*(fVertexMin+fVertexMax));
    fEvent->SetEventPlane(.5*(fEventPlaneMin+fEventPlaneMax));
    // parse the tracks
    ParseTracks(event);

    // write the tree and perform cleanup
    PushToTTree();
    return kTRUE;
}
//_____________________________________________________________________________
void AliGMFMixingManager::ParseTracks()
{
    // parse tracks
    AliGMFTTreeTrack* track(0x0);

    for(Int_t i(0), i < fMultiplicityMin; i++) {
        // then stage a track
        track = GetNextCachedTrackFromEvent(i);
        AliGMFTTreeTrack* acceptedTrack = new((*fTrackArray)[acceptedTracks]) AliGMFTTreeTrack();
        acceptedTrack->SetPt(track->Pt());
        acceptedTrack->SetEta(track->Eta());
        acceptedTrack->SetPhi(track->Phi());
        acceptedTrack->SetCharge(track->Charge());
    }
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
