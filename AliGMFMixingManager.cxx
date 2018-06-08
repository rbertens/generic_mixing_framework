// author: redmer alexander bertens (rbertens@cern.ch)

#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TArrayI.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1.h"
#include "TH1I.h"

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
    fMaxEvents(-1),
    fBufferPadding(-1),
    fMaxEventsPerFile(1e9),
    fHowToChooseMultiplicity(kUseDistribution),
    fSplittingThreshold(1e9),
    fSplitTrackPt(3),
    fMultInvariantSplitting(kTRUE),
    fTree(0x0),
    fEvent(0x0),
    fBufferedEvent(0x0), 
    fTrackArray(0x0),
    fOutputFile(0x0),
    fEventReader(0x0),
    fAutoOverflow(kTRUE),
    fOverflowPosition(-1),
    fEventBufferPosition(0),
    fTrackBufferPosition(0),
    fQAManager(0x0),
    fEventCache(0x0),
    fOnTheFlyMultDist(0x0),
    fRandomMultiplicity(kTRUE)
{
    // default constructor
}
//_____________________________________________________________________________
AliGMFMixingManager::~AliGMFMixingManager() {
    // class destructor - only delete what the manager allocates
    delete fQAManager;    
    delete fEventCache;  
}
//_____________________________________________________________________________
void AliGMFMixingManager::DoQA() {
    // initialize the QA manager
    fQAManager = new AliGMFHistogramManager();
    fQAManager->BookTH1D("fHistRejectedMultiplicity", "counts", 4000, 0, 4000);
    fQAManager->BookTH2D("fHistRejectedMultCent", "counts", "percentile", 100, 0, 4000, 100, 0, 100);
    fQAManager->BookTH1D("fHistRejectedVertex", "cm", 100, -12, 12);
    fQAManager->BookTH1D("fHistRejectedCentrality", "percentile", 100, 0, 100);
    fQAManager->BookTH1D("fHistRejectedEP", "rad", 100, -2, 2);
    fQAManager->BookTH1D("fHistAcceptedMultiplicity", "counts", 4000, 0, 4000);
    fQAManager->BookTH2D("fHistAcceptedMultCent", "counts", "percentile", 100, 0, 4000, 100, 0, 100);
    fQAManager->BookTH1D("fHistAcceptedVertex", "cm", 100, -12, 12);
    fQAManager->BookTH1D("fHistAcceptedCentrality", "percentile", 100, 0, 100);
    fQAManager->BookTH1D("fHistAcceptedEP", "rad", 100, -2, 2);
    fQAManager->BookTH1D("fHistRejectionReason", "0=mult 1=vtx 2=ep 3=cen", 4, 0, 4);
    fQAManager->BookTH1D("fHistUnmixedPt", "#it{p}_{T} (GeV/c)", 100, 0, 20);
    fQAManager->BookTH1D("fHistUnmixedEta", "#eta", 100, -1, 1);
    fQAManager->BookTH1D("fHistUnmixedPhi", "#phi", 100, 0, TMath::TwoPi());
    fQAManager->BookTH2D("fHistUnmixedEtaPhi", "#eta", "#phi", 100, -1, 1, 100, 0, TMath::TwoPi());
    fQAManager->BookTH1D("fHistMixedPt", "#it{p}_{T} (GeV/c)", 100, 0, 20);
    fQAManager->BookTH1D("fHistMixedEta", "#eta", 100, -1, 1);
    fQAManager->BookTH1D("fHistMixedPhi", "#phi", 100, 0, TMath::TwoPi());
    fQAManager->BookTH2D("fHistMixedEtaPhi", "#eta", "#phi", 100, -1, 1, 100, 0, TMath::TwoPi());
    fQAManager->BookTH1D("fHistMixedVertex", "cm", 100, -12, 12);
    fQAManager->BookTH1D("fHistMixedCentrality", "percentile", 100, 0, 100);
    fQAManager->BookTH1D("fHistMixedEventPlane", "#Psi", 100, -4, 4);
    fQAManager->BookTH1D("fHistMixedMultiplicity", "counts", 4000, 0, 4000);
    fQAManager->BookTH1D("fHistMixedMultiplicityNoSplitting", "counts", 4000, 0, 4000);
    fQAManager->BookTH1D("fHistPassesOverData", "data passes", 1, 0, 1);
    
}
//_____________________________________________________________________________
Bool_t AliGMFMixingManager::Initialize() {

    // used to on the fly sample multiplicities from a realistic distribution
    fOnTheFlyMultDist = new TH1I("fOnTheFlyMultDist", "fOnTheFlyMultDist", fMultiplicityMin, fMultiplicityMax, fMultiplicityMax-fMultiplicityMin);
    fOnTheFlyMultDist->SetDirectory(0);

    // initialize output structure
    fOutputFile = new TFile("myMixedEvents.root", "RECREATE");

    fTree = new TTree("tree", "mixed event data");
    fEvent = new AliGMFTTreeHeader();
    fTree->Branch("mixedEvent", &fEvent);

    fTrackArray = new TClonesArray("AliGMFTTreeTrack", fMultiplicityMax);
    fTree->Bronch("mixedTrack", "TClonesArray", &fTrackArray); 

    fTotalEventBuffer = fEventReader->GetNumberOfEvents();

#if VERBOSE > 0
    printf("   ... %i events found,  this can take a while \n", fTotalEventBuffer);
#endif


    return (fTotalEventBuffer > 0); 
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
    printf("   ... created cache (this can allocate a lot of RAM, but only once!)  ... \n ");
#endif

    if(fBufferPadding <= 0) {
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
    } else {
        // calculate the size of the event buffer including the padding
        fBufferPadding = fMultiplicityMax + fMultiplicityMax*(((float)fBufferPadding)/100.);
        fEventCache = new TObjArray(fBufferPadding, 0);
        fEventCache->SetOwner(kTRUE);
#if VERBOSE > 0
        printf(" Buffer padded to %i from %i \n", fBufferPadding, fMultiplicityMax);
#endif
        for(Int_t i(0); i < fBufferPadding; i++) {
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
#if VERBOSE > 0
    printf("   ... done  \n");
#endif
}
//_____________________________________________________________________________
Bool_t AliGMFMixingManager::FillMixingCache(Int_t iCache) {
    // go through the events in the reader until we've found M events that
    // can be used for mixing, caching the index numbers of the eligible events
    // we start at the buffer position

    // if iCache is negative, it means we've already once automatically overflowed the cache
    // we cannot do it twice, so we pretend to run out of data
    if(iCache < 0) return kFALSE;


    AliGMFTTreeTrack* track(0x0);
    AliGMFEventContainer* currentEvent(0x0);
    AliGMFEventContainer* cachedEvent(0x0);
#if VERBOSE > 0
    printf(" ::FillMixingCache:: \n");
    printf("   ... filling cache from buffer position  %i \n", fEventBufferPosition);
#endif

    // only flush the multiplicity cache if this is a fresh mixing cache
    if(!fMultiplicityDist.empty() && fOverflowPosition < 0) fMultiplicityDist.clear();

    while ((currentEvent = fEventReader->GetEvent(fEventBufferPosition))) {
        // the buffer position moves with each event 
        fEventBufferPosition++;
        if(IsSelected(currentEvent)) {
            // this event meets out criteria, we make a local copy of it to the cache
            cachedEvent = static_cast<AliGMFEventContainer*>(fEventCache->At(iCache));
            cachedEvent->FlushAndFill(currentEvent);
            // and shuffle the indices of the tracks
            cachedEvent->ShuffleTrackIndices();
            fOnTheFlyMultDist->Fill(currentEvent->GetMultiplicity());
            fMultiplicityDist.push_back(currentEvent->GetMultiplicity());
            iCache++;
#if VERBOSE > 0
            std::cout << "     - caching event " << iCache << " found at buffer position " << fEventBufferPosition << "\r"; cout.flush();
#endif
            if(fQAManager) {
                // multiplicity is retrieved from the current event, otherwise we just get the buffer mult
                fQAManager->Fill("fHistAcceptedMultiplicity", currentEvent->GetMultiplicity());
                fQAManager->Fill("fHistAcceptedMultCent", currentEvent->GetMultiplicity(), cachedEvent->GetCentrality());
                fQAManager->Fill("fHistAcceptedVertex", cachedEvent->GetZvtx());
                fQAManager->Fill("fHistAcceptedCentrality", cachedEvent->GetCentrality());
                fQAManager->Fill("fHistAcceptedEP", cachedEvent->GetEventPlane());
                for(Int_t i(0); i < fMultiplicityMax; i++) {
                    if((track = cachedEvent->GetTrack(i))) {
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
            fQAManager->Fill("fHistRejectedEP", currentEvent->GetEventPlane());
        }

        // if the cache is full, break the loop
        if(iCache == fMultiplicityMax && fBufferPadding <= 0) break;
        else if(iCache == fBufferPadding) break;
   }
#if VERBOSE > 0
    cout << endl;
#endif

    // TODO if the cache is NOT full, but we reach the end of the read buffer, 
    // fAutoOverflow will determine whether we keep filling the cache, or just
    // let the process end
    if(!currentEvent && !fAutoOverflow) return kFALSE;
    else if (!currentEvent) {
        // remember how many events you originally want to create
        // otherwise you artificially enhance low occurence events
        if(fOverflowPosition < 0) fOverflowPosition = fMultiplicityDist.size();
        // roll back the event buffer position
        fEventBufferPosition = 0;
        // try to resume filling the cache
        if(fQAManager) fQAManager->Fill("fHistPassesOverData", 1);
#if VERBOSE > 0
    printf(" Input event buffer depleted, resetting buffer at cache position %i \n", iCache);
#endif
        FillMixingCache(iCache);
    }
    return kTRUE;
}
//_____________________________________________________________________________
void AliGMFMixingManager::StageCachedEvent(Int_t i) {
    // retrieve the i-th good event and put it in the event buffer 
    fBufferedEvent = static_cast<AliGMFEventContainer*>(fEventCache->At(i));
    //    FillHeaderWithCachedEventInfo();
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

    if(fBufferPadding <= 0) {
        return fBufferedEvent->GetNextTrack();    
    } else {
        AliGMFTTreeTrack* track = fBufferedEvent->GetNextTrack();
        if(!track) {
            // we ran out of tracks in this specific event , so we try an event from the overflow buffer
            Int_t bufferOffset(0);
            while(!track) {
                StageCachedEvent(bufferOffset + fMultiplicityMax);
                track = fBufferedEvent->GetNextTrack();
                bufferOffset++;
                // avoid running out-of-bounds
                if((bufferOffset + fMultiplicityMax) > fBufferPadding) break;
            }
        }
        return track;
    }
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
    Int_t i(0), j(0);

    while(FillMixingCache()) {
        CreateNewEventChunk();
        i+=fMultiplicityMin;
#if VERBOSE > 0
        printf("   - created %i new events \n", i);
#endif
        j+=fMultiplicityMin;
        if(j > fMaxEventsPerFile) {
            // check if we've reached the max number of events for the file, and if so, reset counter and write the output
            WriteCurrentTreeToFile(kTRUE);
            j = 0;
        }
        if(fMaxEvents >= 0 && i > fMaxEvents) {
            WriteCurrentTreeToFile(kTRUE);
            break;
        }

    }

    // 3) write the tree to a file
    Finish();
#if VERBOSE > 0
    if(i > 0) {
        printf(" Event mixer finished and should have written at least %i events and %i tracks \n", i, i*fMultiplicityMin);
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
    Int_t iMixedTracks(0), sampledMultiplicity(0), maxMultChoices(0), splitTracks(0);
    // this outer loop only changes the gobal track index
    // and at the same time serves as event iterator
    // (in 'square' mixing, event i is created by sampling the i-th track
    // from all buffered events
    for(fTrackBufferPosition = 0; fTrackBufferPosition < fMultiplicityMax; fTrackBufferPosition++) {
        // bookkeep the total number of tracks that is added to the array
        iMixedTracks = 0;
        // select a desired multiplicity , try again if zero (not likely)
        if(!fRandomMultiplicity) {
            sampledMultiplicity = fMultiplicityDist.at(fTrackBufferPosition);
        } else {
            sampledMultiplicity = (int)(fOnTheFlyMultDist->GetRandom());
            while(sampledMultiplicity == 0) {
                maxMultChoices++;
                sampledMultiplicity = (int)(fOnTheFlyMultDist->GetRandom());
                if(maxMultChoices == 99) sampledMultiplicity = gRandom->Uniform(fMultiplicityMin, fMultiplicityMax);
            } 
        }
        // enter the track loop
        for(Int_t i(0); i < fMultiplicityMax; i++) {
            // go through all the buffered events i, and take the next
            // 'unused' track from them
            track = GetNextTrackFromEventI(i);
            if(!track)  continue;
            while(track->GetPt() > fSplittingThreshold) {
                // as long as the track pt is too high, create
                // new tracks which have fixed pt
                // and are collinear to the original track
                AliGMFTTreeTrack* mixedTrack = new((*fTrackArray)[iMixedTracks]) AliGMFTTreeTrack();
                mixedTrack->Fill(track);
                mixedTrack->SetPt(fSplitTrackPt);
                track->SetPt(track->GetPt() - fSplitTrackPt);
                // if requested do some qa
                if(fQAManager) {
                    fQAManager->Fill("fHistMixedPt", mixedTrack->GetPt());
                    fQAManager->Fill("fHistMixedEta", mixedTrack->GetEta());
                    fQAManager->Fill("fHistMixedPhi", mixedTrack->GetPhi());
                    fQAManager->Fill("fHistMixedEtaPhi", mixedTrack->GetEta(), mixedTrack->GetPhi());
                }
                iMixedTracks++;
                // tricky part: if we dont want to change energy density, increase the mult
                // of this event by the number of split tracks. this changes the resulting multiplicity
                // distribution, but *not* the sampled one
                if(fMultInvariantSplitting) {
                    splitTracks++;
                    sampledMultiplicity++;
                }
            }
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
            if(iMixedTracks >= sampledMultiplicity) break;
            // check if this was an overflow event, so we might kill the procedure before
            // reaching the max
        }
        // write the tree and perform cleanup
        if(iMixedTracks < sampledMultiplicity) FlushCurrentTTree();
        else {
            if(fQAManager) {
                fQAManager->Fill("fHistMixedMultiplicity", iMixedTracks);
                fQAManager->Fill("fHistMixedMultiplicityNoSplitting", iMixedTracks - splitTracks);
            }
            PushToTTree();
            // check if this is a chunk that was created using automatic overfilling of the
            // mixing buffer. if yes, and if the mixing buffer is as large as the mixed buffer
            // terminate mixing
#if VERBOSE > 0
            std::cout << "     - writing mixed event " << fTrackBufferPosition << "\r"; cout.flush();
#endif

            if(fAutoOverflow && (fTrackBufferPosition >= fOverflowPosition)) {
                // not only do we need to return, but we also need to tell the manager that we will stop mixing now
                // for this we set the max event counter to 0, which will trigger the mixing to exit
                fMaxEvents = 0;
                return;
            }
        }
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
void AliGMFMixingManager::FlushCurrentTTree() {
    fTrackArray->Clear();
}
//_____________________________________________________________________________
void AliGMFMixingManager::Finish() {
    // write and close the files
    WriteCurrentTreeToFile(kFALSE);
    if(fQAManager) {
        fQAManager->StoreRatio("fHistAcceptedMultiplicity", "fHistMixedMultiplicity", "fHistMultRatios");
        fQAManager->StoreManager("mixingQA.root");
    }
}
