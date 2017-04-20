// event reader class
#include "AliGMFEventReader.h"
#include "AliGMFEventContainer.h"
#include "../filter/AliGMFTTreeTrack.h"
#include "../filter/AliGMFTTreeHeader.h"

#include "TChain.h"
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

    // create pointers for the branches in the chain
    Int_t check(0);
    check += fInputChain->SetBranchAddress("event", &fHeader);
    check += fInputChain->SetBranchAddress("track", &fTracks);
    if(check%5!=0) {
        printf(" Warning, input chain contains unexpected input \n");
        return kFALSE;
    }

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

