 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFTrackCuts.h"
#include "AliVTrack.h"
#include "AliAODTrack.h"

ClassImp(AliGMFTrackCuts);

AliGMFTrackCuts::AliGMFTrackCuts() : TObject(),
    fFilterBit(0)
{
  // default constructor
}
 
Bool_t AliGMFTrackCuts::IsSelected(AliVTrack* track) {
    // see if the track passes criteria
    fAODTrack = reinterpret_cast<AliAODTrack*>(track);

    if (!fAODTrack) return kFALSE;
    if (!fAODTrack->TestFilterBit(fFilterBit)) return kFALSE;
    return kTRUE;
}
