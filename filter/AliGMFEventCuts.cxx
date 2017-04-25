 // author: redmer alexander bertens (rbertnens@cern.ch)

#include "AliGMFEventCuts.h"
#include "AliVEvent.h"
#include "AliVVertex.h"
#include "AliCentrality.h"

ClassImp(AliGMFEventCuts);

AliGMFEventCuts::AliGMFEventCuts() : TObject()
{
  // default constructor
}

Bool_t AliGMFEventCuts::IsSelected(AliVEvent* event) {
    // check event cuts
    fCurrentEvent = event;
    
    if(!fCurrentEvent) return kFALSE;
    if(!PassesCentralitySelection()) return kFALSE;
    if(!PassesVertexSelection()) return kFALSE;
    // all is good, jay
    return kTRUE;
}

Bool_t AliGMFEventCuts::PassesCentralitySelection() {
    // check centrality criteria
    Float_t centrality(fCurrentEvent->GetCentrality()->GetCentralityPercentile("V0M"));
    if (centrality > 0 && centrality < 90) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFEventCuts::PassesVertexSelection() {
    // check vertex criteria
    if(TMath::Abs(fCurrentEvent->GetPrimaryVertex()->GetZ()) > 10) return kFALSE;
    return kTRUE;
}
