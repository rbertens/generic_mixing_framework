// simple jet finder on generic input vector
//
// Redmer Alexander Bertens, 2017 (UTK, CERN)

#include "fastjet/Selector.hh"
//#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/ClusterSequenceAreaBase.hh"


#include <iostream>
#include <vector>

#include "TH1F.h"
#include "TFile.h"
#include "TMath.h"

#include "AliGMFEventContainer.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFSimpleJetFinder.h"
#include "AliGMFHistogramManager.h"
#include "AliGMFTTreeEventCuts.h"

ClassImp(AliGMFSimpleJetFinder)

using namespace std;

//_____________________________________________________________________________
AliGMFSimpleJetFinder::AliGMFSimpleJetFinder() : TObject(),
    fDoBackgroundSubtraction(kFALSE),
    fJetResolution(.3),
    fLeadingHadronPt(.1),
    fEventCuts(0x0),
    fHistogramManager(0x0)
{
    // default constructor
}
    
//_____________________________________________________________________________
Bool_t AliGMFSimpleJetFinder::Initialize() {
    // initialize
   fHistogramManager = new AliGMFHistogramManager();
   
   // create the histograms (for now here)
   fHistogramManager->BookTH1F("fHistJetPt", "p_{T}^{jet}", 100, 0, 100);
   fHistogramManager->BookTH1F("fHistMultiplicity", "track multiplicity", 1000, 0, 2000);
   fHistogramManager->BookTH1F("fHistRho", "#rho", 100, 0, 150);
   fHistogramManager->BookTH2F("fHistJetPtArea", "p_{T}^{jet}", "area", 100, 0, 100, 100, 0, 1);
   fHistogramManager->BookTH2F("fHistJetEtaPhi", "#eta^{jet}", "#phi^{jet}", 100, -1, 1, 100, 0, TMath::TwoPi());
   fHistogramManager->BookTH1F("fHistVertex", "cm", 100, -12, 12);
   fHistogramManager->BookTH1F("fHistCentrality", "percentile", 100, 0, 100);
   fHistogramManager->BookTH1F("fHistEventPlane", "#Psi", 100, -4, 4);

   return kTRUE;

}
//_____________________________________________________________________________

Bool_t AliGMFSimpleJetFinder::AnalyzeEvent(AliGMFEventContainer* event) {
    // called for each event
    
    // check if event cuts are required, and if so, if the event passes
    if(fEventCuts) {
        if(!fEventCuts->IsSelected(event)) return kFALSE;
    }
    fHistogramManager->Fill("fHistVertex", event->GetZvtx());
    fHistogramManager->Fill("fHistEventPlane", event->GetEventPlane());
    fHistogramManager->Fill("fHistCentrality", event->GetCentrality());

    // define the fastjet input vector and create a pointer to a track
    std::vector <fastjet::PseudoJet> fjInputVector;
    AliGMFTTreeTrack* track(0x0);
    Double_t totalE;


    // store some event info
    fHistogramManager->Fill("fHistMultiplicity", event->GetNumberOfTracks());

    for(Int_t i(0); i < event->GetNumberOfTracks(); i++) {
        track = event->GetTrack(i);
        if(track) {
            totalE = track->GetPt()*TMath::CosH(track->GetEta());
            if (totalE <= 0) continue;
            totalE = totalE*totalE + .14*.14;   // assume pion mass for each track
            fastjet::PseudoJet fjInputProtoJet(
                    track->GetPt()*TMath::Cos(track->GetPhi()), 
                    track->GetPt()*TMath::Sin(track->GetPhi()), 
                    track->GetPt()*TMath::SinH(track->GetEta()), 
                    TMath::Sqrt(totalE));
            fjInputProtoJet.set_user_index(i);
            fjInputVector.push_back(fjInputProtoJet);
        }
    }

    // setup the jet finder for signal and background jets
    fastjet::GhostedAreaSpec     ghostSpec(.9, 1, 0.001);
    fastjet::Strategy            strategy = fastjet::Best;
    fastjet::RecombinationScheme recombScheme = fastjet::BIpt_scheme;
    fastjet::AreaType            areaType = fastjet::active_area;
    fastjet::AreaDefinition      areaDef = fastjet::AreaDefinition(areaType, ghostSpec);

    // some bookkeeping
    fastjet::RangeDefinition range(fJetResolution-.9, .9-fJetResolution, 0, 2.*fastjet::pi);
    fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, fJetResolution, recombScheme, strategy);
    fastjet::JetDefinition jetDefRho(fastjet::kt_algorithm, fJetResolution, recombScheme, strategy);

    // feed the protojets to fastjet
    fastjet::ClusterSequenceArea clusterSeq(fjInputVector, jetDef, areaDef);
    fastjet::ClusterSequenceArea clusterSeqRho(fjInputVector, jetDefRho, areaDef);

    // get the jets
    std::vector <fastjet::PseudoJet> inclusiveJets = clusterSeq.inclusive_jets();

    for (UInt_t iJet = 0; iJet < inclusiveJets.size(); iJet++) {
        if (!range.is_in_range(inclusiveJets[iJet])) continue;
        // loop over constituents to apply leading hadron cut
        std::vector<fastjet::PseudoJet> constituents = clusterSeq.constituents(inclusiveJets[iJet]);
        Double_t maxpt(0);
        for(UInt_t i(0); i < constituents.size(); i++) {
            if(constituents[i].perp() > maxpt) maxpt = constituents[i].perp();
        }
        // check jet area and constituent requirement
        if(maxpt < fLeadingHadronPt) continue;
        if(inclusiveJets[iJet].area() < .56*TMath::Pi()*fJetResolution*fJetResolution) continue;
        
        fHistogramManager->Fill(
                "fHistJetPt", 
                inclusiveJets[iJet].perp()
                );
        fHistogramManager->Fill(
                "fHistJetPtArea", 
                inclusiveJets[iJet].perp(),
                inclusiveJets[iJet].area()
                );
        fHistogramManager->Fill(
                "fHistJetEtaPhi",
                inclusiveJets[iJet].eta(),
                inclusiveJets[iJet].phi()
                );
    }

    std::vector <fastjet::PseudoJet> backgroundJets = clusterSeqRho.inclusive_jets();
    Double_t rhoVector[backgroundJets.size()];

    for (UInt_t iJet = 0; iJet < backgroundJets.size(); iJet++) {
        // TODO first pass to exclude n number of hard jets
        if (!range.is_in_range(inclusiveJets[iJet])) continue;
        rhoVector[iJet] = backgroundJets[iJet].perp() / backgroundJets[iJet].area();
    }
    Double_t rho = TMath::Median(backgroundJets.size(), rhoVector);
    fHistogramManager->Fill(
            "fHistRho", 
            rho
            );

    return kTRUE;
}
//_____________________________________________________________________________

Bool_t AliGMFSimpleJetFinder::Finalize(TString name) {

    fHistogramManager->StoreManager(Form("%s.root", name.Data()));
    return kTRUE;

}
