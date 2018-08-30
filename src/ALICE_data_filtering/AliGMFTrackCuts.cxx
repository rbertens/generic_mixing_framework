 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFTrackCuts.h"
#include "AliVTrack.h"
#include "AliAODTrack.h"

ClassImp(AliGMFTrackCuts);

AliGMFTrackCuts::AliGMFTrackCuts() : TObject(),
    fFilterBit(768),
    fMinPt(.2),
    fMaxPt(100),
    fMinEta(-.9),
    fMaxEta(.9)
{
  // default constructor
}
 
Bool_t AliGMFTrackCuts::IsSelected(AliVTrack* track) {
    // see if the track passes criteria
    fAODTrack = static_cast<AliAODTrack*>(track);

    if (!fAODTrack) return kFALSE;
    if (!fAODTrack->TestFilterBit(fFilterBit)) return kFALSE;
    if (fAODTrack->Pt() < fMinPt || fAODTrack->Pt() > fMaxPt) return kFALSE;
    if (fAODTrack->Eta() < fMinEta || fAODTrack->Eta() > fMaxEta) return kFALSE;

    return kTRUE;
}
