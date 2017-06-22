// event reader class
#include "AliGMFEventReader.h"
#include "AliGMFEventContainer.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFTTreeHeader.h"
//#include "AliLog.h"

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

    Int_t check(0);

    // check for 'regular' data
    check += fInputChain->SetBranchAddress("event", &fHeader);
    check += fInputChain->SetBranchAddress("track", &fTracks);
    if(check != 0) {
        // else check for mixed events
        check = 0;
        check += fInputChain->SetBranchAddress("mixedEvent", &fHeader);
        check += fInputChain->SetBranchAddress("mixedTrack", &fTracks);
        // see if there is still hope
        if(check != 0) {
            printf("Couldn't find input branches in the file you supplied \n");
            return kFALSE; 
        } else printf(" \n   Found proper input - please ignore E-TChain messages (if any)\n");
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

