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
 
AliGMFTTreeTrack::~AliGMFTTreeTrack(){}
