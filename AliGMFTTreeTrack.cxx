// author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFTTreeTrack.h"

ClassImp(AliGMFTTreeTrack);

using namespace std;

AliGMFTTreeTrack::AliGMFTTreeTrack() : TObject(),
    fPt          (0.),
    fEta         (0.),
    fPhi         (0.),
    fCharge      (0.),
    fUsed        (kFALSE),
    fFilled      (kFALSE),
    fNumber      (0)
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
    fFilled = kTRUE;
    fNumber = track->GetNumber();
}
//_____________________________________________________________________________
void AliGMFTTreeTrack::Reset() {
    // fill current track with info from supplied track
    fPt = -1;
    fEta = -1;
    fPhi = -1;
    fCharge = -1;
    fUsed = kFALSE;
    fFilled = kFALSE;
    fNumber = 0;
}
//_____________________________________________________________________________
void AliGMFTTreeTrack::PrintInfo() {
    printf(" pt %.4f \t eta %.4f \t phi %.4f \n", fPt, fEta, fPhi);
}
