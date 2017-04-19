// event reader class
#include "AliGMFEventReader.h"
#include "AliGMFEventContainer.h"
#include "../filter/AliGMFTTreeTrack.h"
#include "../filter/AliGMFTTreeHeader.h"

#include "TChain.h"
#include "TClonesArray.h"

ClassImp(AliGMFEventReader)

//-----------------------------------------------------------------------------
AliGMFEventReader::AliGMFEventReader() : TObject()
{
    // ctor
}

//-----------------------------------------------------------------------------
Bool_t AliGMFEventReader::Initialize() {
    // initialize the reader
    if(!fInputChain) {
        printf(" No input chain, aborting \n");
        return kFALSE;
    }

    // create pointers for the branches in the chain
    AliGMFTTreeHeader* fHeader = 0x0;
    fInputChain->SetBranchAddress("event", &fHeader);
    TClonesArray* fTracks = 0x0;
    fInputChain->SetBranchAddress("track", &fTracks);
}
//-----------------------------------------------------------------------------
void AliGMFEventReader::PrintEventSummary() {
    // print some test information of the current event
    printf(" Current event number %i out of %i \n", fCurrentEvent, fEvents);
    printf(" fTracks %p \n", fTracks);
    if(fTracks) printf(" - contains %i tracks \n", fTracks->GetEntries());
}
//-----------------------------------------------------------------------------
void AliGMFEventReader::SetInputChain(TChain* c) {
    fInputChain = c;
    Initialize();
    fEvents = fInputChain->GetEntries();
}
//-----------------------------------------------------------------------------
AliGMFEventContainer* AliGMFEventReader::GetEvent(Int_t i) {
    // provide a container that points to the ith event in the chain
    fCurrentEvent = i;
    fInputChain->GetEntry(fCurrentEvent);
    return new AliGMFEventContainer(fHeader, fTracks);
}

