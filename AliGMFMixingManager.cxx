// author: redmer alexander bertens (rbertens@cern.ch)

#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TArrayI.h"
#include "TMath.h"
#include "TRandom.h"

//#include "AliLog.h"

#include "AliGMFMixingManager.h"
#include "AliGMFEventReader.h"
#include "AliGMFEventContainer.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFTTreeHeader.h"
#include "AliGMFHistogramManager.h"

#if VERBOSE > 0
#include<iostream>
using namespace std;
#endif

ClassImp(AliGMFMixingManager);

//_____________________________________________________________________________
AliGMFMixingManager::AliGMFMixingManager() : TObject(),
    fMultiplicityMin(1), 
    fMultiplicityMax(-1),
    fVertexMin(1),
    fVertexMax(-1),
    fEventPlaneMin(1),
    fEventPlaneMax(-1),
    fCentralityMin(1),
    fCentralityMax(-1),
    fMaxEventsPerFile(1e9),
    fHowToChooseMultiplicity(kUseRandom),
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
    fQAManager->BookTH2F("fHistRejectedMultCent", "counts", "percentile", 100, 0, 2000, 100, 0, 100);
    fQAManager->BookTH1F("fHistRejectedVertex", "cm", 100, -12, 12);
    fQAManager->BookTH1F("fHistRejectedCentrality", "percentile", 100, 0, 100);
    fQAManager->BookTH1F("fHistAcceptedMultiplicity", "counts", 1000, 0, 2000);
    fQAManager->BookTH2F("fHistAcceptedMultCent", "counts", "percentile", 100, 0, 2000, 100, 0, 100);
    fQAManager->BookTH1F("fHistAcceptedVertex", "cm", 100, -12, 12);
    fQAManager->BookTH1F("fHistAcceptedCentrality", "percentile", 100, 0, 100);
    fQAManager->BookTH1F("fHistRejectionReason", "0=mult 1=vtx 2=ep 3=cen", 4, 0, 4);
    fQAManager->BookTH1F("fHistUnmixedPt", "#it{p}_{T} (GeV/c)", 100, 0, 20);
    fQAManager->BookTH1F("fHistUnmixedEta", "#eta", 100, -1, 1);
    fQAManager->BookTH1F("fHistUnmixedPhi", "#phi", 100, 0, TMath::TwoPi());
    fQAManager->BookTH2F("fHistUnmixedEtaPhi", "#eta", "#phi", 100, -1, 1, 100, 0, TMath::TwoPi());
    fQAManager->BookTH1F("fHistMixedPt", "#it{p}_{T} (GeV/c)", 100, 0, 20);
    fQAManager->BookTH1F("fHistMixedEta", "#eta", 100, -1, 1);
    fQAManager->BookTH1F("fHistMixedPhi", "#phi", 100, 0, TMath::TwoPi());
    fQAManager->BookTH2F("fHistMixedEtaPhi", "#eta", "#phi", 100, -1, 1, 100, 0, TMath::TwoPi());
    fQAManager->BookTH1F("fHistMixedVertex", "cm", 100, -12, 12);
    fQAManager->BookTH1F("fHistMixedCentrality", "percentile", 100, 0, 100);
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

    fTotalEventBuffer = fEventReader->GetNumberOfEvents();

#if VERBOSE > 0
    printf("   ... %i events found,  this can take a while \n", fTotalEventBuffer);
#endif

}
//_____________________________________________________________________________
void AliGMFMixingManager::InitializeMixingCache() {
    // the mixing is performed 'per chunk' of M events, where M corresponds to the maximum multiplicity of
    // the mixed classes . in a first step, we go through the input chain, and check which events in the input
    // chain fulfill our mixing criteria, these are stored the first in the 'cache' as a bookkeeping utility
    // memory for this is allocated in this routine
#if VERBOSE > 0
    printf(" ::InitializeMixingCache:: \n");
#endif

    if(fMultiplicityMax < 1) {
        printf(" Maximum multiplicity is too low, aborting \n");
        return;
    }

#if VERBOSE > 0
    printf("   ... created cache (this can allocate a lot of RAM)  ... ");
#endif

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
#if VERBOSE > 0
    printf("   ... done  \n");
#endif
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
#if VERBOSE > 0
    printf(" ::FillMixingCache:: \n");
    printf("   ... filling cache from buffer position  %i \n", fEventBufferPosition);
#endif

    while ((currentEvent = fEventReader->GetEvent(fEventBufferPosition))) {
        // the buffer position moves with each event 
        fEventBufferPosition++;
        if(IsSelected(currentEvent)) {
            // this event meets out criteria, we make a local copy of it to the cache
            cachedEvent = static_cast<AliGMFEventContainer*>(fEventCache->At(iCache));
            cachedEvent->Fill(currentEvent);
            iCache++;
#if VERBOSE > 0
            std::cout << "     - caching event " << iCache+1 << " at buffer position " << fEventBufferPosition << "\r"; cout.flush();
#endif
            if(fQAManager) {
                fQAManager->Fill("fHistAcceptedMultiplicity", currentEvent->GetMultiplicity());
                fQAManager->Fill("fHistAcceptedMultCent", currentEvent->GetMultiplicity(), currentEvent->GetCentrality());
                fQAManager->Fill("fHistAcceptedVertex", currentEvent->GetZvtx());
                fQAManager->Fill("fHistAcceptedCentrality", currentEvent->GetCentrality());
                for(Int_t i(0); i < fMultiplicityMin; i++) {
                    if((track = currentEvent->GetTrack(i))) {
                        fQAManager->Fill("fHistUnmixedPt", track->GetPt());
                        fQAManager->Fill("fHistUnmixedEta", track->GetEta());
                        fQAManager->Fill("fHistUnmixedPhi", track->GetPhi());
                        fQAManager->Fill("fHistUnmixedEtaPhi", track->GetEta(), track->GetPhi());
                    }
                }
            }
        } else if (fQAManager) {
            fQAManager->Fill("fHistRejectedVertex", currentEvent->GetZvtx());
            fQAManager->Fill("fHistRejectedMultiplicity", currentEvent->GetMultiplicity());
            fQAManager->Fill("fHistRejectedCentrality", currentEvent->GetCentrality());
            fQAManager->Fill("fHistRejectedMultCent", currentEvent->GetMultiplicity(), currentEvent->GetCentrality());
        }

        // if the cache is full, break the loop
        if(iCache + 1 == fMultiplicityMax) break;
    }
#if VERBOSE > 0
    cout << endl;
#endif

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
        fEvent->SetCentrality(fBufferedEvent->GetCentrality());
        // and fill the qa hists TODO make this more elegant
        if(gRandom->Uniform(0,fMultiplicityMin) < 1) {
            fQAManager->Fill("fHistMixedVertex", fBufferedEvent->GetZvtx());
            fQAManager->Fill("fHistMixedEventPlane", fBufferedEvent->GetEventPlane());
            fQAManager->Fill("fHistMixedCentrality", fBufferedEvent->GetCentrality());
        }
    }
}
//_____________________________________________________________________________
AliGMFTTreeTrack* AliGMFMixingManager::GetNextTrackFromEventI(Int_t i) {
    // get the next track from the i-th cached event

    // first stage the i-th event
    StageCachedEvent(i);

    // then retrieve the track buffer position for this track and get the track itself
    // TODO
    // small hack; see if scrambling the index fixes the jet finder issue


    //AliGMFTTreeTrack* track(fBufferedEvent->GetTrack(fTrackBufferPosition));
    Int_t rand = gRandom->Uniform(0, fMultiplicityMax);
    
    AliGMFTTreeTrack* track(fBufferedEvent->GetTrack(rand));
    return track;

}
//_____________________________________________________________________________
Int_t AliGMFMixingManager::DoPerChunkMixing() {
    // the heart of this manager
#if VERBOSE > 0
    printf(" ::DoPerCunkMixing:: \n");
#endif

    // 0) general initialization
    Initialize();

    // 1) initialize the mixing cache
    InitializeMixingCache();
    if(!fEventCache) return -1;


    // 2) create new events, loop exits when end of true evens is reached
    //    mixing only starts when the buffer is 100% full
    //    produces chunks of M mixed events
#if VERBOSE > 0
    Int_t i(0), j(0);
#endif

    while(FillMixingCache()) {
        CreateNewEventChunk();
#if VERBOSE > 0
        i+=fMultiplicityMin;
        printf("   - created %i new events \n", i);
#endif
        j+=fMultiplicityMin;
        if(j > fMaxEventsPerFile) {
            // check if we've reached the max number of events for the file, and if so, reset counter and write the output
            WriteCurrentTreeToFile(kTRUE);
            j = 0;
        }
    }

    // 3) write the tree to a file
    Finish();
#if VERBOSE > 0
    if(i > 0) {
        printf(" Event mixer finished and should have written %i events and %i tracks \n", i, i*fMultiplicityMin);
    } else {
        printf(" The mixer couldn't cache sufficient candidate events for mixing. \n");
        printf(" Try re-running with less stringent event selection criteria. \n");
    }
#endif

    // return success
    return 0;
}

//_____________________________________________________________________________ 
Bool_t AliGMFMixingManager::IsSelected(AliGMFEventContainer* event) {
    // check if this event meets the criteria for mixing
    Bool_t pass = kTRUE;
    if(!event) return kFALSE;
    if(event->GetMultiplicity() > fMultiplicityMax || event->GetMultiplicity() < fMultiplicityMin) {
        pass = kFALSE;
        if(fQAManager) fQAManager->Fill("fHistRejectionReason", 0);
    }
    if(event->GetZvtx() < fVertexMin || event->GetZvtx() > fVertexMax) {
        pass = kFALSE;
        if(fQAManager) fQAManager->Fill("fHistRejectionReason", 1);
    }
    if(event->GetEventPlane() > fEventPlaneMax || event->GetEventPlane() < fEventPlaneMin) {
        pass = kFALSE;
        if(fQAManager) fQAManager->Fill("fHistRejectionReason", 2);
    }
    if(event->GetCentrality() > fCentralityMax || event->GetCentrality() < fCentralityMin) {
        pass = kFALSE;
        if(fQAManager) fQAManager->Fill("fHistRejectionReason", 3);
    }
    return pass;
}
//_____________________________________________________________________________
void AliGMFMixingManager::CreateNewEventChunk() 
{
#if VERBOSE > 0
    printf(" ::CreateNewEventChunk:: \n");
#endif

    // and then get the tracks
    AliGMFTTreeTrack* track(0x0);
    Int_t iMixedTracks(0);


    switch (fHowToChooseMultiplicity) {
        case kUseMinimum : {
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
                        fQAManager->Fill("fHistMixedEtaPhi", mixedTrack->GetEta(), mixedTrack->GetPhi());
                    }
                    iMixedTracks++;
                }
                // write the tree and perform cleanup
                PushToTTree();
            }
        } break;
         case kUseRandom : {
            // this outer loop only changes the gobal track index
            // and at the same time serves as event iterator
            // (in 'square' mixing, event i is created by sampling the i-th track
            // from all buffered events
            Int_t mult(0);
            for(fTrackBufferPosition = 0; fTrackBufferPosition < fMultiplicityMin; fTrackBufferPosition++) {
                // bookkeep the total number of tracks that is added to the array
                iMixedTracks = 0;
                // enter the track loop
                mult = gRandom->Uniform(fMultiplicityMin, fMultiplicityMax);
                for(Int_t i(0); i < mult; i++) {
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
                        fQAManager->Fill("fHistMixedEtaPhi", mixedTrack->GetEta(), mixedTrack->GetPhi());
                    }
                    iMixedTracks++;
                }
                // write the tree and perform cleanup
                PushToTTree();
            }
        } break;
        default : break;
    }
}
//_____________________________________________________________________________
void AliGMFMixingManager::WriteCurrentTreeToFile(Bool_t createNewOutputStructures) {
    // write current ttree to file
    fTree->Write();
    fOutputFile->Close();
    delete fOutputFile;

    // and creates a new output structure if requested
    if(createNewOutputStructures) {
        fOutputFile = new TFile(Form("myMixedEvents_%i.root", fEventBufferPosition), "RECREATE");
        fTree = new TTree("tree", "mixed event data");
        fEvent = new AliGMFTTreeHeader();
        fTree->Branch("mixedEvent", &fEvent);

        fTrackArray = new TClonesArray("AliGMFTTreeTrack", 1000);
        fTree->Bronch("mixedTrack", "TClonesArray", &fTrackArray); 
    }
}
//_____________________________________________________________________________
void AliGMFMixingManager::PushToTTree() {
    // push info to tree and do cleanup for next iteration
    fTree->Fill();
    fTrackArray->Clear();
}
//_____________________________________________________________________________
void AliGMFMixingManager::Finish() {
    // write and close the files
    WriteCurrentTreeToFile(kFALSE);
    if(fQAManager) fQAManager->StoreManager("mixingQA.root");
}
