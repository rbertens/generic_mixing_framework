 // author: redmer alexander bertens (rbertens@cern.ch)

#include "AliGMFTTreeHeader.h"

ClassImp(AliGMFTTreeHeader)
AliGMFTTreeHeader::AliGMFTTreeHeader(): TObject(),
  fZvtx(0),
  fEventPlane(0),
  fEventID(-1),
  fUsed(kFALSE),
  fMultiplicity(0)
{
  // default constructor
}

//_________________________________________________________
AliGMFTTreeHeader::~AliGMFTTreeHeader(){}

//_____________________________________________________________________________
void AliGMFTTreeHeader::Fill(AliGMFTTreeHeader* event) {
   // fill current header with info from another header
   fZvtx = event->GetZvtx();
   fEventPlane = event->GetEventPlane();
   fEventID = event->GetEventID();
   fUsed = event->GetUsed();
   fMultiplicity = event->GetMultiplicity();
}
