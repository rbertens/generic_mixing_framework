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
