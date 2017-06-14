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
    fEventBufferPosition(0),
    fTrackBufferPosition(0),
    fQAManager(0x0),
    fEventCache(0x0)
{
    // default constructor
}
//_____________________________________________________________________________
AliGMFMixingManager::~AliGMFMixingManager() {
    // class destructor - only delete what the manager allocates
/*    delete fOutputFile;
    delete fBufferedEvent;
    delete fOutputFile;   */
    delete fQAManager;    
    delete fEventCache;  
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
    fQAManager->BookTH1F("fHistMixedVertex", "cm", 100, -12, 12);
    fQAManager->BookTH1F("fHistMixedEventPlane", "#Psi", 100, -4, 4);
}
//_____________________________________________________________________________
Bool_t AliGMFMixingManager::Initialize() {

    fOutputFile = new TFile("myMixedEvents.root", "RECREATE");

    fTree = new TTree("tree", "mixed event data");
    fEvent = new AliGMFTTreeHeader();
    fTree->Branch("mixedEvent", &fEvent);

    fTrackArray = new TClonesArray("AliGMFTTreeTrack", 1000);
    fTree->Bronch("mixedTrack", "TClonesArray", &fTrackArray); 

#if VERBOSE > 0
    printf(" > %i events found ... this can take a while \n", fEventReader->GetNumberOfEvents());
#endif

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
        // allocate the track buffer
        TClonesArray* trackBuffer = new TClonesArray("AliGMFTTreeTrack", fMultiplicityMax);
        for(Int_t j(0); j < fMultiplicityMax; j++) {
            new((*trackBuffer)[j]) AliGMFTTreeTrack();
        }
        // allocate full event buffer
        fEventCache->AddAt(new AliGMFEventContainer(
                    new AliGMFTTreeHeader(),
                    trackBuffer,
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

    while ((currentEvent = fEventReader->GetEvent(fEventBufferPosition))) {
        // the buffer position moves with each event 
        fEventBufferPosition++;
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
    FillHeaderWithCachedEventInfo();
}
//_____________________________________________________________________________
void AliGMFMixingManager::FillHeaderWithCachedEventInfo() {
    // as the function title suggests
    if(fBufferedEvent) {
        fEvent->SetZvtx(fBufferedEvent->GetZvtx());
        fEvent->SetEventPlane(fBufferedEvent->GetEventPlane());
        // and fill the qa hists
        fQAManager->Fill("fHistMixedVertex", fBufferedEvent->GetZvtx());
        fQAManager->Fill("fHistMixedEventPlane", fBufferedEvent->GetEventPlane());
    }
}
//_____________________________________________________________________________
AliGMFTTreeTrack* AliGMFMixingManager::GetNextTrackFromEventI(Int_t i) {
    // get the next track from the i-th cached event

    // first stage the i-th event
    StageCachedEvent(i);

    // then retrieve the track buffer position for this track and get the track itself
    AliGMFTTreeTrack* track(fBufferedEvent->GetTrack(fTrackBufferPosition));

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
#if VERBOSE > 0
    Int_t i(0);
#endif

    while(FillMixingCache()) {
        CreateNewEventChunk();
#if VERBOSE > 0
        i+=fMultiplicityMin;
        printf("   - created %i new events \n", i);
#endif

    }

    // 3) write the tree to a file
    Finish();
#if VERBOSE > 0
    printf(" Event mixer finished and should have written %i events and %i tracks \n", i, i*fMultiplicityMin);
#endif
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

    // this outer loop only changes the gobal track index
    // and at the same time serves as event iterator
    // (in 'square' mixing, event i is created by sampling the i-th track
    // from all buffered events
    for(fTrackBufferPosition = 0; fTrackBufferPosition < fMultiplicityMin; fTrackBufferPosition++) {
        // bookkeep the total number of tracks that is added to the array
        iMixedTracks = 0;
        // enter the track loop
        for(Int_t i(0); i < fMultiplicityMin; i++) {
            // go through all the buffered events i, and take the next
            // 'unused' track from them
            track = GetNextTrackFromEventI(i);
            //            // the track should always be there ... but ok
            //            if(!track) continue;
            // build the new track and fill it
            AliGMFTTreeTrack* mixedTrack = new((*fTrackArray)[iMixedTracks]) AliGMFTTreeTrack();
            mixedTrack->Fill(track);
            // if requested do some qa
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
