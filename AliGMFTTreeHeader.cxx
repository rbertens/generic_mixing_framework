 // author: redmer alexander bertens (rbertens@cern.ch)

#include "AliGMFTTreeHeader.h"

ClassImp(AliGMFTTreeHeader)
AliGMFTTreeHeader::AliGMFTTreeHeader(): TObject(),
  fZvtx(0),
  fEventPlane(0),
  fEventID(-1),
  fUsed(kFALSE),
  fCentrality(-1),
  fMultiplicity(0),
  fRunNumber(-1)
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
   fCentrality = event->GetCentrality();
   fRunNumber = event->GetRunNumber();
}
//_____________________________________________________________________________
void AliGMFTTreeHeader::Reset() {
   // fill current header with info from another header
   fZvtx = -999;
   fEventPlane = -999;
   fEventID = -1;
   fUsed = kFALSE;
   fMultiplicity = 0;
   fCentrality = -1;
   fRunNumber = -1;
}
