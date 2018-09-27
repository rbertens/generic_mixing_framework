 // author: redmer alexander bertens (rbertens@cern.ch)

#include "AliGMFTTreeHeader.h"

ClassImp(AliGMFTTreeHeader)
AliGMFTTreeHeader::AliGMFTTreeHeader(): TObject(),
  fZvtx(-999),
  fEventPlane(-999),
  fEventPlane3(-999),
  fCentrality(-1),
  fRunNumber(-1),
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
   fEventPlane3 = event->GetEventPlane3();
   fEventID = event->GetEventID();
   fUsed = event->GetUsed();
   fMultiplicity = event->GetMultiplicity();
   fCentrality = event->GetCentrality();
   fRunNumber = event->GetRunNumber();
}
//_____________________________________________________________________________
void AliGMFTTreeHeader::Reset() {
   // reset header 
   fZvtx = -999;
   fEventPlane = -999;
   fEventPlane3 = -999;
   fEventID = -1;
   fUsed = kFALSE;
   fMultiplicity = 0;
   fCentrality = -1;
   fRunNumber = -1;
}
