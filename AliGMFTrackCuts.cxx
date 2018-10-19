 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFTrackCuts.h"
#include "AliVTrack.h"
#include "AliAODTrack.h"

ClassImp(AliGMFTrackCuts);

AliGMFTrackCuts::AliGMFTrackCuts() : TObject(),
    fFilterBit(32),
    fMinPt(.2),
    fMaxPt(100),
    fMinEta(-.9),
    fMaxEta(.9)
{
  // default constructor
}
  
void AliGMFTrackCuts::Diagnose(AliVTrack* track) {
    // see if the track passes criteria
    fAODTrack = static_cast<AliAODTrack*>(track);

    if (!fAODTrack) { printf(" No track found \n"); return; }
    if (!fAODTrack->TestFilterBit(fFilterBit)) {printf(" Filterbit test failed \n, set bit was %i", fAODTrack->GetFilterMap()); return; }
    if (fAODTrack->Pt() < fMinPt || fAODTrack->Pt() > fMaxPt) {printf("Pt not OK \n"); return; }
    if (fAODTrack->Eta() < fMinEta || fAODTrack->Eta() > fMaxEta) {printf("Eta not OK \n"); return; }

    printf(" Track selected\n" );
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
