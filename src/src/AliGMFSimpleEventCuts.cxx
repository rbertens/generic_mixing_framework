 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFSimpleEventCuts.h"
#include "AliGMFEventContainer.h"

ClassImp(AliGMFSimpleEventCuts);

AliGMFSimpleEventCuts::AliGMFSimpleEventCuts() : TObject(),
    fCheckMultiplicity(kFALSE),
    fMultiplicityMin(1), 
    fMultiplicityMax(-1),
    fCheckVertex(kFALSE),
    fVertexMin(1),
    fVertexMax(-1),
    fCheckEventPlane(kFALSE),
    fEventPlaneMin(1),
    fEventPlaneMax(-1),
    fCheckCentrality(kFALSE),
    fCentralityMin(1),
    fCentralityMax(-1),
    fCheckRunNumber(kFALSE),
    fRunNumberMin(0),
    fRunNumberMax(ULONG_MAX)
{
  // default constructor
}

void AliGMFSimpleEventCuts::Diagnose() {
    // check why an event doesn't pass the seleciton criteria
    if(!fCurrentEvent) printf(" > No event found \n"); 
    if(!PassesCentralitySelection()) printf(" Failed centrality check  \n"); 
    if(!PassesVertexSelection()) printf(" Failed vertex check  \n");
    if(!PassesEventPlaneSelection()) printf(" Failed event plane check  \n");
    if(!PassesMultiplicitySelection()) printf(" Failed multiplicity check  \n");
    if(!PassesRunNumberSelection()) printf(" Failed run number check  \n");
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
    if(!fCheckCentrality) return kTRUE;
    if(fCurrentEvent->GetCentrality() > fCentralityMin && fCurrentEvent->GetCentrality() < fCentralityMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFSimpleEventCuts::PassesVertexSelection() {
    // check vertex criteria
    if(!fCheckVertex) return kTRUE;
    if(fCurrentEvent->GetZvtx() > fVertexMin && fCurrentEvent->GetZvtx() < fVertexMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFSimpleEventCuts::PassesEventPlaneSelection() {
    // check event plane criteria
    if(!fCheckEventPlane) return kTRUE;
    if(fCurrentEvent->GetEventPlane() > fEventPlaneMin && fCurrentEvent->GetEventPlane() < fEventPlaneMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFSimpleEventCuts::PassesMultiplicitySelection() {
    // check multiplicity criteria
    if(!fCheckMultiplicity) return kTRUE;
    if(fCurrentEvent->GetMultiplicity() > fMultiplicityMin && fCurrentEvent->GetMultiplicity() < fMultiplicityMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFSimpleEventCuts::PassesRunNumberSelection() {
    if(!fCheckRunNumber) return kTRUE;
    if(fCurrentEvent->GetRunNumber() > fRunNumberMin && fCurrentEvent->GetRunNumber() < fRunNumberMax) return kTRUE;
    return kFALSE;
}
