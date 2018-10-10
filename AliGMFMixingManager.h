#ifndef VERBOSE
#define VERBOSE 0
#endif

#ifndef AliGMFMixingManager_H
#define AliGMFMixingManager_H

#include <vector>

#include <TObject.h>

#include "AliGMFEventContainer.h"

class TTree;
class TH1;
class AliGMFTTreeHeader;
class AliGMFTTreeTrack;
class TClonesArray;
class TFile;
class TArrayI;
class TObjArray;
class AliGMFEventReader;
class AliGMFEventContainer;
class AliGMFHistogramManager;

class AliGMFMixingManager : public TObject {


    public:
        AliGMFMixingManager();
        virtual     ~AliGMFMixingManager();

        // core functions
        Bool_t      Initialize();
        Int_t       DoPerChunkMixing();

        // setters - ranges
        void SetMultiplicityRange(Int_t min, Int_t max) {
            fMultiplicityMin = min;
            fMultiplicityMax = max;
        }
        void SetVertexRange(Float_t min, Float_t max) {
            fVertexMin = min;
            fVertexMax = max;
        }
        void SetEventPlaneRange(Float_t min, Float_t max) {
            fEventPlaneMin = min;
            fEventPlaneMax = max;
        }
        void SetEventPlane3Range(Float_t min, Float_t max) {
            fEventPlane3Min = min;
            fEventPlane3Max = max;
        }

        void SetCentralityRange(Float_t min, Float_t max) {
            fCentralityMin = min;
            fCentralityMax = max;
        }

        // setters - behavior
        void SetSplittingForTracksWithPtHigherThan(Float_t pt) {
            fSplittingThreshold = pt;
        }

        void SetRandomMultiplicity(Bool_t r) {
            fRandomMultiplicity = r;
        }

        void SetSplitTrackPt(Float_t pt) {
            fSplitTrackPt = pt;
        }
        void SetMultInvariantSplitting(Bool_t b)    {fMultInvariantSplitting = b;}

        // setters - IO
        void SetMaxEvents(Int_t e)                  {fMaxEvents = e;}
        void SetMaxEventsPerFile(Int_t e)           {fMaxEventsPerFile = e;}
        void SetAllowBufferPadding(Int_t o)         {fBufferPadding = o;}

        void SetEventReader(AliGMFEventReader* r)   {fEventReader = r;}
        void SetAutoOverflow(Bool_t r)               {fAutoOverflow = r;}
        void DoQA();

    private:

        Bool_t      IsSelected(AliGMFEventContainer* event);
        void        InitializeMixingCache();
        Bool_t      FillMixingCache(Int_t iCache = 0);
        void        StageCachedEvent(Int_t i);
        void        FillHeaderWithCachedEventInfo();
        AliGMFTTreeTrack*   GetNextTrackFromEventI(Int_t i);
//        AliGMFTTreeTrack*   GetRandomTrackFromEventI(Int_t i) {return static_cast<AliGMFTTreeTrack*>(0x0);}   // to be implemented
        void        CreateNewEventChunk();
        void        WriteCurrentTreeToFile(Bool_t createNewOutputStructures);
        void        PushToTTree();
        void        FlushCurrentTTree();
        void        Finish();

        Int_t       fMultiplicityMin;   // minimum multiplicity
        Int_t       fMultiplicityMax;   // maximum multiplicity
        Float_t     fVertexMin;         // minimum vertexz
        Float_t     fVertexMax;         // maximum vertexz
        Float_t     fEventPlaneMin;     // minimum event plane angle
        Float_t     fEventPlaneMax;     // maximum event plane angle
        Float_t     fEventPlane3Min;    // minimum event plane 3 angle
        Float_t     fEventPlane3Max;    // maximum event plane 3 angle
        Float_t     fCentralityMin;     // minimum event centrality
        Float_t     fCentralityMax;     // maximum event centrality
        Int_t       fMaxEvents;         // maximum number of mixed events that will be generated
        Int_t       fBufferPadding;     // percentage of padding that is attached to the event buffer (for high multiplicity classes)
        Int_t       fMaxEventsPerFile;  // maximum number of mixed events written per file

        Float_t     fSplittingThreshold;// tracks with pt > this will be split collinearly
        Float_t     fSplitTrackPt;      // pt of tracks that are split off    
        Bool_t      fMultInvariantSplitting;        // set to false if a split track should change the multiplicity

        // data structures for mixed event output
        TTree*                      fTree;                  //! output data
        AliGMFTTreeHeader*          fEvent;                 //! event header
        AliGMFEventContainer*       fBufferedEvent;         //! buffered real event 
        TClonesArray*               fTrackArray;            //! track container
        TFile*                      fOutputFile;            //! output file

        // misc
        AliGMFEventReader*          fEventReader;           // event reader
        Bool_t                      fAutoOverflow;          // automatic overflow of read buffer when cache cannot be filled
        Int_t                       fOverflowPosition;      // cache position at which overflow started
        Int_t                       fEventBufferPosition;   //! global buffer position
        Int_t                       fTrackBufferPosition;   //! 'walks' trough the track cache 
        AliGMFHistogramManager*     fQAManager;             // run QA 
        TObjArray*                  fEventCache;            // event cache 
        Int_t                       fTotalEventBuffer;      //! total number of events
        TH1*                        fOnTheFlyMultDist;      //! on the fly multiplicity distribution
        std::vector<int>            fMultiplicityDist;      // multiplicity distribution of current buffer
        Bool_t                      fRandomMultiplicity;    // choose mult randomly from input dist

        ClassDef(AliGMFMixingManager, 1);

};

#endif
