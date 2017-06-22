 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFTTreeEventCuts.h"
#include "AliCentrality.h"
#include "AliGMFEventContainer.h"

ClassImp(AliGMFTTreeEventCuts);

AliGMFTTreeEventCuts::AliGMFEventCuts() : TObject(),
    fMultiplicityMin(1), 
    fMultiplicityMax(-1),
    fVertexMin(1),
    fVertexMax(-1),
    fEventPlaneMin(1),
    fEventPlaneMax(-1),
    fCentralityMin(1),
    fCentralityMax(-1)
{
  // default constructor
}

Bool_t AliGMFTTreeEventCuts::IsSelected(AliGMFEventContainer* event) {
    // check event cuts
    fCurrentEvent = event;
    
    if(!fCurrentEvent) return kFALSE;
    if(!PassesCentralitySelection()) return kFALSE;
    if(!PassesVertexSelection()) return kFALSE;
    if(!PassesEventPlaneSelection()) return kFALSE;
    // all is good, jay
    return kTRUE;
}

Bool_t AliGMFTTreeEventCuts::PassesCentralitySelection() {
    // check centrality criteria
    if(fCurrentEvent->GetCentrality() > fCentralityMin && fCurrentEvent->GetCentrality() < fCentralityMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFTTreeEventCuts::PassesVertexSelection() {
    // check vertex criteria
    if(fCurrentEvent->GetZvtx() > fVertexMin && fCurrentEvent->GetZvtx() < fVertexMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFTTreeEventCuts::PassesEventPlaneSelection() {
    // check event plane criteria
    if(fCurrentEvent->GetEventPlane() > fEventPlaneMin && fCurrentEvent->GetEventPlane() < fEventPlaneMax) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFTTreeEventCuts::PassesMultiplicitySelection() {
    // check multiplicity criteria
    if(fCurrentEvent->GetMultiplicity() > fMultiplicityMin && fCurrentEvent->GetMultiplicity() < fMultiplicityMax) return kTRUE;
    return kFALSE;
}

