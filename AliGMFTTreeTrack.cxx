// author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFTTreeTrack.h"

ClassImp(AliGMFTTreeTrack);

AliGMFTTreeTrack::AliGMFTTreeTrack() : TObject(),
    fPt          (0.),
    fEta         (0.),
    fPhi         (0.),
    fCharge      (0.),
    fUsed        (kFALSE)
{
  // default constructor
}
//_____________________________________________________________________________ 
AliGMFTTreeTrack::~AliGMFTTreeTrack(){}
//_____________________________________________________________________________
void AliGMFTTreeTrack::Fill(AliGMFTTreeTrack* track) {
    // fill current track with info from supplied track
    fPt = track->GetPt();
    fEta = track->GetEta();
    fPhi = track->GetPhi();
    fCharge = track->GetCharge();
    fUsed = track->GetUsed();
}
