 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFSimpleTrackCuts.h"
#include "AliGMFTTreeTrack.h"

ClassImp(AliGMFSimpleTrackCuts)

AliGMFSimpleTrackCuts::AliGMFSimpleTrackCuts() : TObject(),
    fCurrentTrack(0x0),
    fTrackMinPt(0),
    fTrackMaxPt(100)
{
  // default constructor
}

Bool_t AliGMFSimpleTrackCuts::IsSelected(AliGMFTTreeTrack* track) {
    // check event cuts
    fCurrentTrack = track;
    
    if(!fCurrentTrack) return kFALSE;
    
    if(!PassesPtCut()) return kFALSE;
    return kTRUE;
}

Bool_t AliGMFSimpleTrackCuts::PassesPtCut() {
    if(fCurrentTrack->GetPt() < fTrackMinPt || fCurrentTrack->GetPt() > fTrackMaxPt) return kFALSE;
    return kTRUE;
}
