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

#include "reader/AliGMFEventContainer.h"
#include "filter/AliGMFTTreeTrack.h"
#include "jetfinder/AliGMFDummyJetFinder.h"
#include "tools/AliGMFHistogramManager.h"

ClassImp(AliGMFDummyJetFinder)

using namespace std;

//_____________________________________________________________________________
AliGMFDummyJetFinder::AliGMFDummyJetFinder() : TObject(),
    fDoBackgroundSubtraction(kFALSE),
    fJetResolution(.3),
    fLeadingHadronPt(.1),
    fHistogramManager(0x0),
    fHistJetPt(0x0)
{
    // default constructor
}
    
//_____________________________________________________________________________
Bool_t AliGMFDummyJetFinder::Initialize() {
    // initialize
   fHistogramManager = new AliGMFHistogramManager();
   
   // create the histograms (for now here)
   fHistogramManager->BookTH1F("fHistJetPt", "p_{T}^{jet}", 100, 0, 100);

   return kTRUE;

}
//_____________________________________________________________________________

Bool_t AliGMFDummyJetFinder::AnalyzeEvent(AliGMFEventContainer* event) {
    // called for each event
    
    // define the fastjet input vector and create a pointer to a track
    std::vector <fastjet::PseudoJet> fjInputVector;
    AliGMFTTreeTrack* track(0x0);
    Double_t totalE;

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

    // setup the jet finder
    fastjet::GhostedAreaSpec     ghostSpec(.9, 1, 0.001);
    fastjet::Strategy            strategy = fastjet::Best;
    fastjet::RecombinationScheme recombScheme = fastjet::BIpt_scheme;
    fastjet::AreaType            areaType = fastjet::active_area;
    fastjet::AreaDefinition      areaDef = fastjet::AreaDefinition(areaType, ghostSpec);

    // some bookkeeping
    fastjet::RangeDefinition range(fJetResolution-.9, .9-fJetResolution, 0, 2.*fastjet::pi);
    fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, fJetResolution, recombScheme, strategy);

    // feed the protojets to fastjet
    fastjet::ClusterSequenceArea clusterSeq(fjInputVector, jetDef, areaDef);

    // get the jets
    std::vector <fastjet::PseudoJet> inclusiveJets = clusterSeq.inclusive_jets();
/*
    // define background strategy (applied if requested)
    fastjet::JetMedianBackgroundEstimator bge;
    fastjet::ClusterSequenceArea *clustSeqBG = 0;

    if (fDoBackgroundSubtraction) {
        fastjet::JetDefinition jetDefBG(fastjet::kt_algorithm, fJetResolution, recombScheme, strategy);
        fastjet::Selector BGSelector = fastjet::SelectorStrip(2*fJetResolution);
        BGSelector.set_reference(inclusiveJets);
        fastjet::ClusterSequenceArea clustSeqBG(fjInputVector, jetDefBG,areaDef);

        BGSelector.set_jets(inclusiveJets);
        clustSeqBG = new fastjet::ClusterSequenceArea(fjInputs, jetDefBG,areaDef);
        vector <fastjet::PseudoJet> BGJets = clustSeqBG->inclusive_jets();

        bge.set_selector(BGSelector);
        bge.set_jets(BGJets);
    }
*/


    // jet finding is done, store info in histograms
    Float_t eta_lead_jet=0; 
    Float_t jet_eta= 0;
    Float_t jet_phi= 0;
    Float_t jet_pt= 0;
    Float_t jet_pt_bgsub= 0;

    for (UInt_t iJet = 0; iJet < inclusiveJets.size(); iJet++) {
        if (!range.is_in_range(inclusiveJets[iJet])) continue;
        // loop over constituents to apply leading hadron cut
        std::vector<fastjet::PseudoJet> constituents = clusterSeq.constituents(inclusiveJets[iJet]);
        double maxpt(0);
        for(UInt_t i(0); i < constituents.size(); i++) {
            if(constituents[i].perp() > maxpt) maxpt = constituents[i].perp();
        }
        if(maxpt < fLeadingHadronPt) continue;

        // if we pass the lh cut
        jet_pt = inclusiveJets[iJet].perp();
        jet_eta = inclusiveJets[iJet].eta();
/*        float rho = 0;

        if (fDoBackgroundSubtraction) {
            rho = bge.rho(inclusiveJets[iJet]);
            float jet_area = clustSeqCh.area(inclusiveJets[iJet]);
            float bgsub = rho*jet_area;
            jet_pt_bgsub = jet_pt - bgsub;
        }
        else*/ jet_pt_bgsub = jet_pt;

        // fill histograms
        fHistogramManager->Fill("fHistJetPt", jet_pt_bgsub);
    }

    return kTRUE;
}
//_____________________________________________________________________________

Bool_t AliGMFDummyJetFinder::Finalize() {

    fHistogramManager->StoreManager("jetFinderOutput.root");
    return kTRUE;

}
