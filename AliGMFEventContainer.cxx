// for the track shuffler
#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>

// root includes
#include "TClonesArray.h"

// custom includes
#include "AliGMFEventContainer.h"
#include "AliGMFTTreeHeader.h"
#include "AliGMFTTreeTrack.h"

ClassImp(AliGMFEventContainer)

//------------------------------------------------------------------------
AliGMFEventContainer::AliGMFEventContainer( 
        AliGMFTTreeHeader* event,
        TClonesArray* array, Int_t eventID) : TObject()
{

    // link the event
    fHeader = event;
    fTracks = array;
    if(fHeader) fHeader->SetEventID(eventID);
}
//------------------------------------------------------------------------
void AliGMFEventContainer::SetEvent(
        AliGMFTTreeHeader* event,
        TClonesArray* tracks, Int_t eventID) {
    // set event, only public access to avoid mixing tracks and headers
    fHeader = event;
    fTracks = tracks;
    if(fHeader) fHeader->SetEventID(eventID);
}
//-----------------------------------------------------------------------------
Bool_t AliGMFEventContainer::Fill(AliGMFEventContainer* event) {
    // fill the current event with info from another event
    // just fills, does *not* create objects !!!
    if(!event) return kFALSE;

    // fill the event header
    fHeader->Fill(event->GetHeader());

    // fill the tracks
    for(Int_t i(0); i < event->GetNumberOfTracks(); i++) {
        if(GetTrack(i) && event->GetTrack(i)) {
            GetTrack(i)->Fill(event->GetTrack(i));
        } else printf(" Warning, tried to fill track %i but memory is not allocated - skipping track ! \n ");
    }
    return kTRUE;
}
//-----------------------------------------------------------------------------
void AliGMFEventContainer::Flush() {
    // reset this event
    if(GetHeader()) GetHeader()->Reset();
    AliGMFTTreeTrack* track(0x0);
    // important is that the track flag 'filled'
    // is set to kFALSE
    for(Int_t i(0); i < GetNumberOfTracks(); i++) {
        track = GetTrack(i);
        if(track) track->Reset();
    }
}
//-----------------------------------------------------------------------------
Int_t AliGMFEventContainer::FlushAndFill(AliGMFEventContainer* event) {
    // clear out the event (no deallocation) and fill it with new event info
    Flush();
    // fill the new events
    Fill(event);
    // remove 'holes' from the index array
    return FlushOutZeroes();

}
//-----------------------------------------------------------------------------
Int_t AliGMFEventContainer::FlushOutZeroes() {
    // remove any zeroes from the track index buffer

    // put numbers from the back in the holes :)

    return 0;
}
//-----------------------------------------------------------------------------
void AliGMFEventContainer::SetUsed(Bool_t used) {
    // mark the header as read
    fHeader->SetUsed(used);
}
//-----------------------------------------------------------------------------
void AliGMFEventContainer::PrintEventSummary() {
    // print some test information of the current event
    printf("\n Current event number %i \n", fHeader->GetEventID());
    if(fTracks) printf(" - contains %i tracks \n", fTracks->GetEntries());
    printf(" - status %i \n", (int)(fHeader->GetUsed()));
}
//_____________________________________________________________________________
void AliGMFEventContainer::Dump() const {
    // 'recursive' dump of contents
    TObject::Dump();
    if(fHeader) {
        printf(" Dumping event header \n");
        fHeader->Dump();
    }
    if(fTracks) {
        printf(" Dump array with %i tracks \n", fTracks->GetEntries());
        fTracks->Dump();
    }
}
//_____________________________________________________________________________
void AliGMFEventContainer::ShuffleTrackIndices() {
    // the order of tracks is not necessarily 'random'
    // this function shuffles the indices in the track so that the
    // new index array is a random bijection of the old index array

    // reset the current shuffled indices  or create a new map
    // if there's no old in memory
    ResetTrackIndices();

    // shuffle the indices
    std::random_shuffle(
            fTrackIndexMap.begin(), 
            fTrackIndexMap.end());
}
//_____________________________________________________________________________
void AliGMFEventContainer::ResetTrackIndices() {
    // reset the track indices to non-shuffled values
    if(!fTrackIndexMap.empty()) {
        fTrackIndexMap.clear();
    }
    for (Int_t i = 0; i < GetNumberOfTracks(); i++) fTrackIndexMap.push_back(i);
}
//_____________________________________________________________________________
AliGMFTTreeTrack* AliGMFEventContainer::GetTrack(Int_t i) {
    // return the i-th track of the event

    if(fTrackIndexMap.empty()) {
        return static_cast<AliGMFTTreeTrack*>(fTracks->At(i));
    } else {
        return static_cast<AliGMFTTreeTrack*>(fTracks->At(fTrackIndexMap[i]));
    }
}

