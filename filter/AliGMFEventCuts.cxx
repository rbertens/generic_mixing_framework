 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFEventCuts.h"
#include "AliVEvent.h"

ClassImp(AliGMFEventCuts);

AliGMFEventCuts::AliGMFEventCuts() : TObject()
{
  // default constructor
}

Bool_t AliGMFEventCuts::IsSelected(AliVEvent* event) {
    // check event cuts
    if(event) return kTRUE;
    return kFALSE;
}
