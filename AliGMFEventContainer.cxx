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

