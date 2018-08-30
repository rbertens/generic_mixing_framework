#ifndef AliGMFSimpleTrackCuts_H
#define AliGMFSimpleTrackCuts_H

#include <TObject.h>

class AliGMFTTreeTrack;

class AliGMFSimpleTrackCuts : public TObject {

 public:
    AliGMFSimpleTrackCuts();

    Bool_t IsSelected(AliGMFTTreeTrack* track);
    Bool_t PassesPtCut();

    // setters
    void        SetTrackMinPt(Float_t m)        { fTrackMinPt = m; }
    void        SetTrackMaxPt(Float_t m)        { fTrackMaxPt = m; }


 private:

    AliGMFTTreeTrack*   fCurrentTrack;  // current track
    Float_t             fTrackMinPt;    // minimum track pt
    Float_t             fTrackMaxPt;    // maximum track pt
    ClassDef(AliGMFSimpleTrackCuts, 1); // Help class

};

#endif
