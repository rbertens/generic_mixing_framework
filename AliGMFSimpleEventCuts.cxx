 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFSimpleEventCuts.h"
#include "AliGMFEventContainer.h"

ClassImp(AliGMFSimpleEventCuts);

AliGMFSimpleEventCuts::AliGMFSimpleEventCuts() : TObject(),
    fMultiplicityMin(1), 
    fMultiplicityMax(-1),
    fVertexMin(1),
    fVertexMax(-1),
    fEventPlaneMin(1),
    fEventPlaneMax(-1),
    fCentralityMin(1),
    fCentralityMax(-1),
    fRunNumberMin(-1),
    fRunNumberMax(1e9)
{
  // default constructor
}

Bool_t AliGMFSimpleEventCuts::IsSelected(AliGMFEventContainer* event) {
    // check event cuts
    fCurrentEvent = event;
    
    if(!fCurrentEvent) return kFALSE;
    if(!PassesCentralitySelection()) return kFALSE;
    if(!PassesVertexSelection()) return kFALSE;
    if(!PassesEventPlaneSelection()) return kFALSE;
    if(!PassesMultiplicitySelection()) return kFALSE;
    if(!PassesRunNumberSelection()) return kFALSE;
    // all is good, jay
    return kTRUE;
}

Bool_t AliGMFSimpleEventCuts::PassesCentralitySelection() {
    // check centrality criteria
    if(fCurrentEvent->GetCentrality() > fCentralityMin && fCurrentEvent->GetCentrality() < fCentralityMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFSimpleEventCuts::PassesVertexSelection() {
    // check vertex criteria
    if(fCurrentEvent->GetZvtx() > fVertexMin && fCurrentEvent->GetZvtx() < fVertexMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFSimpleEventCuts::PassesEventPlaneSelection() {
    // check event plane criteria
    if(fCurrentEvent->GetEventPlane() > fEventPlaneMin && fCurrentEvent->GetEventPlane() < fEventPlaneMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFSimpleEventCuts::PassesMultiplicitySelection() {
    // check multiplicity criteria
    if(fCurrentEvent->GetMultiplicity() > fMultiplicityMin && fCurrentEvent->GetMultiplicity() < fMultiplicityMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFSimpleEventCuts::PassesRunNumberSelection() {
    if(fCurrentEvent->GetRunNumber() > fRunNumberMin && fCurrentEvent->GetRunNumber() < fRunNumberMax) return kTRUE;
    return kFALSE;
}
