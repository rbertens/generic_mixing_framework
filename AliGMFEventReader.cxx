// event reader class
#include "AliGMFEventReader.h"
#include "AliGMFEventContainer.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFTTreeHeader.h"

#include "TChain.h"
#include "TBranch.h"
#include "TClonesArray.h"

ClassImp(AliGMFEventReader)

//-----------------------------------------------------------------------------
AliGMFEventReader::AliGMFEventReader() : TObject(),
    fInputChain(0x0),
    fTracks(0x0),
    fHeader(0x0),
    fEventContainer(0x0),
    fCurrentEvent(0),
    fEvents(0) {
    // ctor
}
//-----------------------------------------------------------------------------
AliGMFEventReader::AliGMFEventReader(TChain* c) : TObject(),
    fInputChain(0x0),
    fTracks(0x0),
    fHeader(0x0),
    fEventContainer(0x0),
    fCurrentEvent(0),
    fEvents(0) {
    // constructor with chain
    SetInputChain(c);
}
//-----------------------------------------------------------------------------
Bool_t AliGMFEventReader::Initialize() {
    // initialize the reader
    if(!fInputChain) {
        printf(" No input chain, aborting \n");
        return kFALSE;
    }

    TBranch* eventBranch = fInputChain->GetBranch("event");
    eventBranch->SetAddress(&fHeader);
    eventBranch->SetAutoDelete(kTRUE);

    TBranch* trackBranch = fInputChain->GetBranch("track");
    trackBranch->SetAddress(&fTracks);
    trackBranch->SetAutoDelete(kTRUE);
    
    return kTRUE;
}

//-----------------------------------------------------------------------------
void AliGMFEventReader::SetInputChain(TChain* c) {
    fInputChain = c;
    fEvents = fInputChain->GetEntries();
    Initialize();
}
//-----------------------------------------------------------------------------
void AliGMFEventReader::MoveChainBufferTo(Int_t i) {
    // provide a container that points to the ith event in the chain
    fCurrentEvent = i;
    fInputChain->GetEntry(fCurrentEvent);
}
//-----------------------------------------------------------------------------
AliGMFEventContainer* AliGMFEventReader::GetEvent(Int_t i) {
    // provide a container that points to the ith event in the chain
    if( i-1 > GetNumberOfEvents()) return 0x0;
    MoveChainBufferTo(i);
    if(!fEventContainer) {
        fEventContainer = new AliGMFEventContainer(fHeader, fTracks, i);
    } else {
        fEventContainer->SetEvent(fHeader, fTracks, i);
    }
    return fEventContainer;
}
//-----------------------------------------------------------------------------
void AliGMFEventReader::TouchEvent(Int_t i) {
    // mark the i-th event as opened, for testing purposes
    GetEvent(i)->SetUsed(kTRUE);
}

