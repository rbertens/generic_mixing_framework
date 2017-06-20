 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFTrackCuts.h"
#include "AliVTrack.h"
#include "AliAODTrack.h"

ClassImp(AliGMFTrackCuts);

AliGMFTrackCuts::AliGMFTrackCuts() : TObject(),
    fFilterBit(768)
{
  // default constructor
}
 
Bool_t AliGMFTrackCuts::IsSelected(AliVTrack* track) {
    // see if the track passes criteria
    fAODTrack = static_cast<AliAODTrack*>(track);

    if (!fAODTrack) return kFALSE;
    if (!fAODTrack->TestFilterBit(fFilterBit)) return kFALSE;
    if (fAODTrack->Pt() < .2) return kFALSE;
    if (TMath::Abs(fAODTrack->Eta() > .9)) return kFALSE;
    return kTRUE;
}
