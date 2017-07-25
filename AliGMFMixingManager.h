#ifndef VERBOSE
#define VERBOSE 1
#endif

#ifndef AliGMFMixingManager_H
#define AliGMFMixingManager_H

#include <TObject.h>

#include "AliGMFEventContainer.h"

class TTree;
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

    enum howToChooseMultiplicity {
        kUseMinimum,
        kUseRandom,
        kUseDistribution};

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

    void SetCentralityRange(Float_t min, Float_t max) {
        fCentralityMin = min;
        fCentralityMax = max;
    }

    // setters - behavior
    void SetHowToChooseMultiplicity(howToChooseMultiplicity h) {
        fHowToChooseMultiplicity = h;
    }
        
    
    // setters - IO
    void SetMaxEventsPerFile(Int_t e)           {fMaxEventsPerFile = e;}

    void SetEventReader(AliGMFEventReader* r)   {fEventReader = r;}
    void DoQA();

 private:

    Bool_t      IsSelected(AliGMFEventContainer* event);
    void        InitializeMixingCache();
    Bool_t      FillMixingCache();
    void        StageCachedEvent(Int_t i);
    void        FillHeaderWithCachedEventInfo();
    AliGMFTTreeTrack*   GetNextTrackFromEventI(Int_t i);
    AliGMFTTreeTrack*   GetRandomTrackFromEventI(Int_t i) {;}   // to be implemented
    void        CreateNewEventChunk();
    void        WriteCurrentTreeToFile(Bool_t createNewOutputStructures);
    void        PushToTTree();
    void        Finish();

    Int_t       fMultiplicityMin;   // minimum multiplicity
    Int_t       fMultiplicityMax;   // maximum multiplicity
    Float_t     fVertexMin;         // minimum vertexz
    Float_t     fVertexMax;         // maximum vertexz
    Float_t     fEventPlaneMin;     // minimum event plane angle
    Float_t     fEventPlaneMax;     // maximum event plane angle
    Float_t     fCentralityMin;     // minimum event centrality
    Float_t     fCentralityMax;     // maximum event centrality
    Int_t       fMaxEventsPerFile;  // maximum number of mixed events written per file

    howToChooseMultiplicity     fHowToChooseMultiplicity;       // how to choose multiplicity

    // data structures for mixed event output
    TTree*                      fTree;                  //! output data
    AliGMFTTreeHeader*          fEvent;                 //! event header
    AliGMFEventContainer*       fBufferedEvent;         //! buffered real event 
    TClonesArray*               fTrackArray;            //! track container
    TFile*                      fOutputFile;            //! output file

    // misc
    AliGMFEventReader*          fEventReader;           // event reader
    Int_t                       fEventBufferPosition;   //! global buffer position
    Int_t                       fTrackBufferPosition;   //! 'walks' trough the track cache 
    AliGMFHistogramManager*     fQAManager;             // run QA 
    TObjArray*                  fEventCache;            // event cache 
    Int_t                       fTotalEventBuffer;      //! total number of events

    ClassDef(AliGMFMixingManager, 1);

};

#endif
