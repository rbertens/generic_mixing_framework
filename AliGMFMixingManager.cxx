 // author: redmer alexander bertens (rbertens@cern.ch)

#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TArrayI.h"
#include "TMath.h"

#include "AliLog.h"

#include "AliGMFMixingManager.h"
#include "AliGMFEventReader.h"
#include "AliGMFEventContainer.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFTTreeHeader.h"
#include "AliGMFHistogramManager.h"

ClassImp(AliGMFMixingManager);

//_____________________________________________________________________________
AliGMFMixingManager::AliGMFMixingManager() : TObject(),
    fMultiplicityMin(1), 
    fMultiplicityMax(-1),
    fVertexMin(1),
    fVertexMax(-1),
    fEventPlaneMin(1),
    fEventPlaneMax(-1),
    fTree(0x0),
    fEvent(0x0),
    fBufferedEvent(0x0), 
    fTrackArray(0x0),
    fOutputFile(0x0),
    fEventReader(0x0),
    fGlobalBufferPosition(0),
    fTrackCacheLexer(0),
    fQAManager(0x0),
    fEventCache(0x0)
{
  // default constructor
}
//_____________________________________________________________________________
void AliGMFMixingManager::DoQA() {
    // initialize the QA manager
    fQAManager = new AliGMFHistogramManager();
    fQAManager->BookTH1F("fHistRejectedMultiplicity", "counts", 1000, 0, 2000);
    fQAManager->BookTH1F("fHistRejectedVertex", "cm", 100, -12, 12);
    fQAManager->BookTH1F("fHistAcceptedMultiplicity", "counts", 1000, 0, 2000);
    fQAManager->BookTH1F("fHistAcceptedVertex", "cm", 100, -12, 12);
    fQAManager->BookTH1F("fHistRejectionReason", "0 cen, 1 vtx, 2 ep", 3, 0, 3);
    fQAManager->BookTH1F("fHistUnmixedPt", "#it{p}_{T} (GeV/c)", 100, 0, 10);
    fQAManager->BookTH1F("fHistUnmixedEta", "#eta", 100, -1, 1);
    fQAManager->BookTH1F("fHistUnmixedPhi", "#phi", 100, 0, TMath::TwoPi());
    fQAManager->BookTH1F("fHistMixedPt", "#it{p}_{T} (GeV/c)", 100, 0, 10);
    fQAManager->BookTH1F("fHistMixedEta", "#eta", 100, -1, 1);
    fQAManager->BookTH1F("fHistMixedPhi", "#phi", 100, 0, TMath::TwoPi());
}
//_____________________________________________________________________________
Bool_t AliGMFMixingManager::Initialize() {

    fOutputFile = new TFile("myMixedEvents.root", "RECREATE");

    fTree = new TTree("tree", "Event data");
    fEvent = new AliGMFTTreeHeader();
    fTree->Branch("mixedEvent", &fEvent);

    fTrackArray = new TClonesArray("AliGMFTTreeTrack", 1000);
    fTree->Bronch("mixedTrack", "TClonesArray", &fTrackArray); 


}
//_____________________________________________________________________________
void AliGMFMixingManager::InitializeMixingCache() {
    // the mixing is performed 'per chunk' of M events, where M corresponds to the maximum multiplicity of
    // the mixed classes . in a first step, we go through the input chain, and check which events in the input
    // chain fulfill our mixing criteria, these are stored the first in the 'cache' as a bookkeeping utility
    // memory for this is allocated in this routine

    if(fMultiplicityMax < 1) AliFatal(" Maximum multiplicity is too low \n");

    fEventCache = new TObjArray(fMultiplicityMax, 0);
    fEventCache->SetOwner(kTRUE);

    for(Int_t i(0); i < fMultiplicityMax; i++) {
        fEventCache->AddAt(new AliGMFEventContainer(
                    new AliGMFTTreeHeader(),
                    new TClonesArray("AliGMFTTreeTrack", fMultiplicityMax),
                    i), // this is the container ID
                    i); // this is the iterator of the object array
    }
}
//_____________________________________________________________________________
Bool_t AliGMFMixingManager::FillMixingCache() {
    // go through the events in the reader until we've found M events that
    // can be used for mixing, caching the index numbers of the eligible events
    // we start at the buffer position
    
    AliGMFTTreeTrack* track(0x0);
    AliGMFEventContainer* currentEvent(0x0);
    AliGMFEventContainer* cachedEvent(0x0);
    Int_t iCache(0);

    while ((currentEvent = fEventReader->GetEvent(fGlobalBufferPosition))) {
        // the buffer position moves with each event 
        fGlobalBufferPosition++;
        if(IsSelected(currentEvent)) {
            // this event meets out criteria, we make a local copy of it to the cache
            cachedEvent = static_cast<AliGMFEventContainer*>(fEventCache->At(iCache));
            cachedEvent->Fill(currentEvent);
            iCache++;
            if(fQAManager) {
                fQAManager->Fill("fHistAcceptedMultiplicity", currentEvent->GetMultiplicity());
                fQAManager->Fill("fHistAcceptedVertex", currentEvent->GetZvtx());
                for(Int_t i(0); i < fMultiplicityMin; i++) {
                    if((track = currentEvent->GetTrack(i))) {
                        fQAManager->Fill("fHistUnmixedPt", track->GetPt());
                        fQAManager->Fill("fHistUnmixedEta", track->GetEta());
                        fQAManager->Fill("fHistUnmixedPhi", track->GetPhi());
                    }
                }
            }
        } else if (fQAManager) {
            fQAManager->Fill("fHistRejectedVertex", currentEvent->GetZvtx());
            fQAManager->Fill("fHistRejectedMultiplicity", currentEvent->GetMultiplicity());
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
    fBufferedEvent = static_cast<AliGMFEventContainer*>(fEventCache->At(i));
}
//_____________________________________________________________________________
AliGMFTTreeTrack* AliGMFMixingManager::GetNextTrackFromEvent(Int_t i) {
    // get the next track from the i-th cached event

    // first stage the i-th event
    StageCachedEvent(i);

    // then retrieve the track buffer position for this track and get the track itself
    AliGMFTTreeTrack* track(fBufferedEvent->GetTrack(fTrackCacheLexer));

    return track;
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
    //    produces chunks of M mixed events
    while(FillMixingCache()) {
        CreateNewEventChunk();
    }

    // 3) write the tree to a file
    Finish();
}

//_____________________________________________________________________________ 
Bool_t AliGMFMixingManager::IsSelected(AliGMFEventContainer* event) {
    // check if this event meets the criteria for mixing
    Bool_t pass = kTRUE;
    if(!event) return kFALSE;
    if(event->GetZvtx() < fVertexMin || event->GetZvtx() > fVertexMax) {
        pass = kFALSE;
        if(fQAManager) fQAManager->Fill("fHistRejectionReason", 1);
    }
    if(event->GetMultiplicity() > fMultiplicityMax || event->GetMultiplicity() < fMultiplicityMin) {
        pass = kFALSE;
        if(fQAManager) fQAManager->Fill("fHistRejectionReason", 0);
    }
    if(event->GetEventPlane() > fEventPlaneMax || event->GetEventPlane() < fEventPlaneMin) {
        pass = kFALSE;
        if(fQAManager) fQAManager->Fill("fHistRejectionReason", 2);
    }

    return pass;
}
//_____________________________________________________________________________
void AliGMFMixingManager::CreateNewEventChunk() 
{
    // and then get the tracks
    AliGMFTTreeTrack* track(0x0);
    Int_t iMixedTracks(0);

    for(fTrackCacheLexer = 0; fTrackCacheLexer < fMultiplicityMin; fTrackCacheLexer++) {
        // store some faux info
        fEvent->SetZvtx(.5*(fVertexMin+fVertexMax));
        fEvent->SetEventPlane(.5*(fEventPlaneMin+fEventPlaneMax));
        iMixedTracks = 0;
        for(Int_t i(0); i < fMultiplicityMin; i++) {
            track = GetNextTrackFromEvent(i);
            if(!track) continue;
            AliGMFTTreeTrack* mixedTrack = new((*fTrackArray)[iMixedTracks]) AliGMFTTreeTrack();
            mixedTrack->SetPt(track->GetPt());
            mixedTrack->SetEta(track->GetEta());
            mixedTrack->SetPhi(track->GetPhi());
            mixedTrack->SetCharge(track->GetCharge());
            if(fQAManager) {
                fQAManager->Fill("fHistMixedPt", mixedTrack->GetPt());
                fQAManager->Fill("fHistMixedEta", mixedTrack->GetEta());
                fQAManager->Fill("fHistMixedPhi", mixedTrack->GetPhi());
            }
            iMixedTracks++;
        }
        // write the tree and perform cleanup
        PushToTTree();
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
    if(fQAManager) fQAManager->StoreManager("mixingQA.root");
}
