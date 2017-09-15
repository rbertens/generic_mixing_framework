// author: redmer alexander bertens (rbertens@cern.ch)

#include "AliGMFEventCuts.h"
#include "AliVEvent.h"
#include "AliAODTrack.h"
#include "AliVVertex.h"
#include "AliCentrality.h"

ClassImp(AliGMFEventCuts);

AliGMFEventCuts::AliGMFEventCuts() : TObject(),
    fCurrentEvent(0x0),
    fCheck2010PileUpRejection(kTRUE)
{
    // default constructor
}

Bool_t AliGMFEventCuts::IsSelected(AliVEvent* event) {
    // check event cuts - slowest check at the end
    fCurrentEvent = event;

    if(!fCurrentEvent) return kFALSE;
    if(!PassesCentralitySelection()) return kFALSE;
    if(!PassesVertexSelection()) return kFALSE;
    if(fCheck2010PileUpRejection &&(!Passes2010PileUpRejection())) return kFALSE;
    // all is good, jay
    return kTRUE;
}

Bool_t AliGMFEventCuts::PassesCentralitySelection() {
    // check centrality criteria
    Float_t centrality(fCurrentEvent->GetCentrality()->GetCentralityPercentile("V0M"));
    if (TMath::Abs(centrality-fCurrentEvent->GetCentrality()->GetCentralityPercentile("TRK")) > 5.) return kFALSE;
    if (centrality > 0 && centrality < 90) return kTRUE;
    return kFALSE;
}

Bool_t AliGMFEventCuts::PassesVertexSelection() {
    // check vertex criteria
    if(TMath::Abs(fCurrentEvent->GetPrimaryVertex()->GetZ()) > 10) return kFALSE;
    if(TMath::Abs(fCurrentEvent->GetPrimaryVertexSPD()->GetZ() - fCurrentEvent->GetPrimaryVertex()->GetZ()) > .5) return kFALSE; 
    return kTRUE;
}

Bool_t AliGMFEventCuts::Passes2010PileUpRejection() {
    // check for pileup via the different between tpc and global multiplicity (SLOW!)
    Float_t multTPC(0.), multGlob(0.);
    Int_t nGoodTracks(fCurrentEvent->GetNumberOfTracks());
    for(Int_t iTracks = 0; iTracks < nGoodTracks; iTracks++) { // fill tpc mult
        AliAODTrack* trackAOD = static_cast<AliAODTrack*>(fCurrentEvent->GetTrack(iTracks));
        if (!trackAOD || !(trackAOD->TestFilterBit(1)) || (trackAOD->Pt() < .2) || (trackAOD->Pt() > 5.0) || (TMath::Abs(trackAOD->Eta()) > .8) || (trackAOD->GetTPCNcls() < 70)  || (trackAOD->GetDetPid()->GetTPCsignal() < 10.0) || (trackAOD->Chi2perNDF() < 0.2)) continue;
        multTPC++;
    }
    for(Int_t iTracks = 0; iTracks < nGoodTracks; iTracks++) { // fill global mult
        AliAODTrack* trackAOD = static_cast<AliAODTrack*>(fCurrentEvent->GetTrack(iTracks));
        if (!trackAOD || !(trackAOD->TestFilterBit(16)) || (trackAOD->Pt() < .2) || (trackAOD->Pt() > 5.0) || (TMath::Abs(trackAOD->Eta()) > .8) || (trackAOD->GetTPCNcls() < 70) || (trackAOD->GetDetPid()->GetTPCsignal() < 10.0) || (trackAOD->Chi2perNDF() < 0.1)) continue;
        Double_t b[2] = {-99., -99.};
        Double_t bCov[3] = {-99., -99., -99.};
        AliAODTrack copy(*trackAOD);
        if (!(copy.PropagateToDCA(fCurrentEvent->GetPrimaryVertex(), fCurrentEvent->GetMagneticField(), 100., b, bCov))) continue;
        if ((TMath::Abs(b[0]) > 0.3) || (TMath::Abs(b[1]) > 0.3)) continue;
        multGlob++;
    }
    if(! (multTPC > (-40.3+1.22*multGlob) && multTPC < (32.1+1.59*multGlob))) return kFALSE;
    return kTRUE;

}

