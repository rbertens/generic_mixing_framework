 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFTrackCuts.h"
#include "AliVTrack.h"

ClassImp(AliGMFTrackCuts);

AliGMFTrackCuts::AliGMFTrackCuts() : TObject()
{
  // default constructor
}
 
Bool_t AliGMFTrackCuts::IsSelected(AliVTrack* track) {
    if (track) return kTRUE;
    return kFALSE;
}
