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
        } else printf(" Warning, tried to fill track %i but memory is not allocated - skipping track ! \n ", i);
    }
    fTrackIterator = -1;
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
    fTrackIterator = -1;
}
//-----------------------------------------------------------------------------
void AliGMFEventContainer::FlushAndFill(AliGMFEventContainer* event) {
    // clear out the event (no deallocation) and fill it with new event info
    Flush();
    // fill the new events
    Fill(event);
}
//-----------------------------------------------------------------------------
Int_t AliGMFEventContainer::FlushOutZeroes() {
    // remove any zeroes from the track index buffer

#ifdef __CLING__

    // if cling is defined, use modern stuff
    auto checkStatus = [&](int i) -> bool {
        return GetTrack(fTrackIndexMap.at(i))->GetFilled();
    };
    std::remove_if(
            fTrackIndexMap.begin(), 
            fTrackIndexMap.end(), 
            checkStatus);
#else

    // first get the size of the index map
    UInt_t lastEntry = fTrackIndexMap.size()-1;
    Int_t tempEntry;
    // loop over all entries in the track map
    for(UInt_t i(0); i < fTrackIndexMap.size(); i++) {
        if(lastEntry <= i) break;
        // check if an index points to an unused track
        if(!(GetTrack(fTrackIndexMap.at(i))->GetFilled())) {
            // if so, find the used track with the highest index number
            while(!GetTrack(fTrackIndexMap.at(lastEntry))->GetFilled()) {
                lastEntry--;
            }
            if(lastEntry > i) {
                // and then swap the used and unused track
                tempEntry = fTrackIndexMap.at(i);
                fTrackIndexMap.at(i) = fTrackIndexMap.at(lastEntry);
                fTrackIndexMap.at(lastEntry) = tempEntry;
                lastEntry--;
            }
        }
    }
#endif

    return fTrackIndexMap.size();
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

    // then order the tracks again, to remove any 'holes'
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
        if(i > GetMultiplicity()) return 0x0;
        return static_cast<AliGMFTTreeTrack*>(fTracks->At(i));
    } else {
        return static_cast<AliGMFTTreeTrack*>(fTracks->At(fTrackIndexMap.at(i)));
    }
}
//_____________________________________________________________________________
AliGMFTTreeTrack* AliGMFEventContainer::GetNextTrack() {
    // return the i-th filled track of the event

    fTrackIterator++;

    // if many tracks are skipped (not filled) these conditions can happen
    if(fTrackIterator > GetMultiplicity()-1 || fTrackIterator < 0) return 0x0;

    AliGMFTTreeTrack* track(0x0);

    // try to get a track
    if(fTrackIndexMap.empty()) {
        track = static_cast<AliGMFTTreeTrack*>(fTracks->At(fTrackIterator));
    } else {
        track = static_cast<AliGMFTTreeTrack*>(fTracks->At(fTrackIndexMap.at(fTrackIterator)));
    }

    // keep trying until it's filled or we reach the end of the buffer
    while(!track->GetFilled()) {
        fTrackIterator++;
        if(fTrackIterator > GetMultiplicity()-1) return 0x0;
        if(fTrackIndexMap.empty()) {
            track = static_cast<AliGMFTTreeTrack*>(fTracks->At(fTrackIterator));
        } else {
            track = static_cast<AliGMFTTreeTrack*>(fTracks->At(fTrackIndexMap.at(fTrackIterator)));
        }
    }
    return track;
}
//_____________________________________________________________________________
void AliGMFEventContainer::ResetTrack(Int_t i) {
    // reset track at index i
    AliGMFTTreeTrack* track = GetTrack(i);
    if(track) track->Reset();
}
//_____________________________________________________________________________
Int_t AliGMFEventContainer::FindEmptyTrack() {
    // find a slot of the tclones array that has an empty track in it,
    // return the index to that element, or -1 when no track is found
    AliGMFTTreeTrack* track(0x0);
    for(Int_t i = 0; i < GetNumberOfTracks(); i++) {
        track = GetTrack(i);
        if(track && !track->GetUsed()) return i;
    }
    return -1;
}
//_____________________________________________________________________________
AliGMFTTreeTrack* AliGMFEventContainer::FindOrCreateEmptyTrack() {
    // find a slot of the tclones array that has an empty track in it,
    // return the index to that element, or add a new track if no track was found
    Int_t trackIdx(FindEmptyTrack());
    if(trackIdx < 0) {
        // create a new track
        trackIdx = GetNumberOfTracks();
        new((*fTracks)[trackIdx]) AliGMFTTreeTrack();
    }
    return GetTrack(trackIdx);
}
//_____________________________________________________________________________
