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

#include "TH1D.h"
#include "TFile.h"
#include "TMath.h"
#include "TRandom.h"

#include "AliGMFEventContainer.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFSimpleJetFinder.h"
#include "AliGMFHistogramManager.h"
#include "AliGMFSimpleEventCuts.h"

ClassImp(AliGMFSimpleJetFinder)

using namespace std;

//_____________________________________________________________________________
AliGMFSimpleJetFinder::AliGMFSimpleJetFinder() : TObject(),
    fDoBackgroundSubtraction(kFALSE),
    fJetResolution(.3),
    fLeadingHadronPt(.1),
    fEventCuts(0x0),
    fTrackCuts(0x0),
    fHistogramManager(0x0),
    fRandomizeEtaPhi(kFALSE),
    fImprintV2(0x0),
    fImprintV3(0x0),
    fSplittingThreshold(1e9),
    fSplitTrackPt(3),
    fRandomizeSplitTrack(kFALSE)
{
    // default constructor
}
    
//_____________________________________________________________________________
Bool_t AliGMFSimpleJetFinder::Initialize() {
    // initialize
   fHistogramManager = new AliGMFHistogramManager(TString(Form("_R%i", (int)(10*fJetResolution))));
   
   // create the histograms (for now here)
   fHistogramManager->BookTH1D("fHistJetPt", "p_{T}^{jet}", 500, 0, 500);
   fHistogramManager->BookTH1D("fHistConstituentPt", "p_{T}^{constituent}", 500, 0, 100);
   fHistogramManager->BookTH1D("fHistJetPtSubtracted", "p_{T}^{jet sub} = p_{T}^{jet} - #rho A ", 500, -130, 370); 
   fHistogramManager->BookTH1D("fHistMultiplicity", "track multiplicity", 1000, 0, 4000);
   fHistogramManager->BookTH1D("fHistRho", "#rho", 100, 0, 150);
   fHistogramManager->BookTH2D("fHistMultiplicityRho", "track multiplicity", "#rho#", 1000, 0, 4000, 100, 0, 150);
   fHistogramManager->BookTH2D("fHistCentralityRho", "centrality", "#rho", 100, 0, 100, 100, 0, 150);

   fHistogramManager->BookTH2D("fHistJetPtArea", "p_{T}^{jet}", "area", 100, 0, 100, 100, 0, 1);
   fHistogramManager->BookTH2D("fHistJetEtaPhi", "#eta^{jet}", "#phi^{jet}", 100, -1, 1, 100, 0, TMath::TwoPi());
   fHistogramManager->BookTH1D("fHistVertex", "cm", 100, -12, 12);
   fHistogramManager->BookTH1D("fHistCentrality", "percentile", 100, 0, 100);
   fHistogramManager->BookTH1D("fHistEventPlane", "#Psi", 100, -4, 4);
   fHistogramManager->BookTH1D("fHistJetFinderSettings", "flag", 3, -.5, 2.5);

   TH1D* settings = static_cast<TH1D*>(fHistogramManager->GetHistogram("fHistJetFinderSettings"));
   settings->GetXaxis()->SetBinLabel(1, "fJetResolution");
   settings->SetBinContent(1, fJetResolution);

   // always make a minimal track cuts object
   if(!fTrackCuts) {
       fTrackCuts = new AliGMFSimpleTrackCuts();
   }

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
    Double_t px(0), py(0), pz(0);
    Int_t j(0);

    // only used when randomizing in eta, phi
    Double_t phi(0), eta(0), pt(0);


    for(Int_t i(0); i < event->GetNumberOfTracks(); i++) {
        track = event->GetTrack(i);
        if(fTrackCuts->IsSelected(track) && track->GetPt() < fSplittingThreshold) {
            // use the tracks directly
            if(fRandomizeEtaPhi) {
                eta = gRandom->Uniform(-.9, 9);
                pt = track->GetPt();
                if(fImprintV2) GenerateV2(phi, eta, pt);
                else if(fImprintV3) GenerateV3(phi, eta, pt);
                else phi = gRandom->Uniform(0, TMath::TwoPi());
                px = pt*TMath::Cos(phi);
                py = pt*TMath::Sin(phi); 
                pz = pt*TMath::SinH(eta); 

            } else {
                px = track->GetPt()*TMath::Cos(track->GetPhi()); 
                py = track->GetPt()*TMath::Sin(track->GetPhi());  
                pz = track->GetPt()*TMath::SinH(track->GetEta()); 
            }
            totalE = px*px+py*py+pz*pz;
            if (!(totalE >  0)) continue;
            fastjet::PseudoJet fjInputProtoJet(
                    px, 
                    py, 
                    pz, 
                    TMath::Sqrt(totalE));
            fjInputProtoJet.set_user_index(j);
            fjInputVector.push_back(fjInputProtoJet);
            fHistogramManager->Fill(
                    "fHistConstituentPt", 
                    track->GetPt()
                    );
            j++;
        } else {
            // in this case we'll split the tracks 
            // either collinearly or randomly
            while(track->GetPt() > fSplittingThreshold) {
                // as long as the track pt is too high, 'create'
                // new tracks which have fixed pt and add them as pseudojet
                if(fRandomizeSplitTrack) {
                    eta = gRandom->Uniform(-.9, 9);
                    if(fImprintV2) GenerateV2(phi, eta, pt);
                    else if(fImprintV3) GenerateV3(phi, eta, pt);
                    else phi = gRandom->Uniform(0, TMath::TwoPi());
                    px = fSplitTrackPt*TMath::Cos(phi);
                    py = fSplitTrackPt*TMath::Sin(phi); 
                    pz = fSplitTrackPt*TMath::SinH(eta); 
                } else {
                    px = fSplitTrackPt*TMath::Cos(track->GetPhi()); 
                    py = fSplitTrackPt*TMath::Sin(track->GetPhi());  
                    pz = fSplitTrackPt*TMath::SinH(track->GetEta()); 
                }
                track->SetPt(track->GetPt() - fSplitTrackPt);

                totalE = px*px+py*py+pz*pz;
                if (!(totalE >  0)) continue;
                fastjet::PseudoJet fjInputProtoJet(
                        px, 
                        py, 
                        pz, 
                        TMath::Sqrt(totalE));
                fjInputProtoJet.set_user_index(j);
                fjInputVector.push_back(fjInputProtoJet);
                fHistogramManager->Fill(
                    "fHistConstituentPt", 
                    fSplitTrackPt
                    );
                j++;
            } 
            // then we just add the remaining pt
            if(fRandomizeSplitTrack) {
                eta = gRandom->Uniform(-.9, 9);
                pt = track->GetPt();
                if(fImprintV2) GenerateV2(phi, eta, pt);
                else if(fImprintV3) GenerateV3(phi, eta, pt);
                else phi = gRandom->Uniform(0, TMath::TwoPi());
                px = pt*TMath::Cos(phi);
                py = pt*TMath::Sin(phi); 
                pz = pt*TMath::SinH(eta); 

            } else {
                px = track->GetPt()*TMath::Cos(track->GetPhi()); 
                py = track->GetPt()*TMath::Sin(track->GetPhi());  
                pz = track->GetPt()*TMath::SinH(track->GetEta()); 
            }
            totalE = px*px+py*py+pz*pz;
            if (!(totalE >  0)) continue;
            fastjet::PseudoJet fjInputProtoJet(
                    px, 
                    py, 
                    pz, 
                    TMath::Sqrt(totalE));
            fjInputProtoJet.set_user_index(j);
            fjInputVector.push_back(fjInputProtoJet);
            fHistogramManager->Fill(
                "fHistConstituentPt", 
                track->GetPt()
                );
            j++;
        }
    }
    fHistogramManager->Fill("fHistMultiplicity", j);
    
    // setup the jet finder for signal and background jets
    fastjet::GhostedAreaSpec     ghostSpec(.9, 1, 0.005, 1, .1, 1e-100);
    fastjet::Strategy            strategy = fastjet::Best;
    fastjet::RecombinationScheme recombScheme = fastjet::BIpt_scheme;
    fastjet::AreaType            areaType = fastjet::active_area;
    fastjet::AreaDefinition      areaDef = fastjet::AreaDefinition(areaType, ghostSpec);

    // some bookkeeping
    // FIXME change range definition to selector 
//    fastjet::Selector range(fastjet::SelectorAbsRapMax(1.-.95*fJetResolution));
    fastjet::RangeDefinition range(fJetResolution-.9, .9-fJetResolution, 0, 2.*fastjet::pi);
    fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, fJetResolution, recombScheme, strategy);
    fastjet::JetDefinition jetDefRho(fastjet::kt_algorithm, fJetResolution, recombScheme, strategy);

    // feed the protojets to fastjet
    fastjet::ClusterSequenceArea clusterSeq(fjInputVector, jetDef, areaDef);
    fastjet::ClusterSequenceArea clusterSeqRho(fjInputVector, jetDefRho, areaDef);

    // get the jets
    std::vector <fastjet::PseudoJet> inclusiveJets = clusterSeq.inclusive_jets();
    std::vector <fastjet::PseudoJet> backgroundJets = clusterSeqRho.inclusive_jets();
 
 
    // do the background stuff
    Double_t rhoVector[backgroundJets.size()];

    for (UInt_t iJet = 0; iJet < backgroundJets.size(); iJet++) {
        // TODO first pass to exclude n number of hard jets
        if (range.is_in_range(inclusiveJets[iJet]) && backgroundJets[iJet].area() > 0) {
            rhoVector[iJet] = backgroundJets[iJet].perp() / backgroundJets[iJet].area();
        }
    }
    Double_t rho = TMath::Median(backgroundJets.size(), rhoVector);
    fHistogramManager->Fill(
            "fHistRho", 
            rho
            );
    fHistogramManager->Fill(
            "fHistMultiplicityRho",
            j, 
            rho
            );
    fHistogramManager->Fill(
            "fHistCentralityRho",
            event->GetCentrality(), 
            rho
            );

    // fill the jet histograms
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
                "fHistJetPtSubtracted",
                inclusiveJets[iJet].perp() - rho * inclusiveJets[iJet].area()
                );
        fHistogramManager->Fill(
                "fHistJetEtaPhi",
                inclusiveJets[iJet].eta(),
                inclusiveJets[iJet].phi()
                );
    }


    return kTRUE;
}
//_____________________________________________________________________________

Bool_t AliGMFSimpleJetFinder::Finalize(TString name) {

    fHistogramManager->StoreManager(Form("%s.root", name.Data()));
    return kTRUE;

}
//_____________________________________________________________________________
void AliGMFSimpleJetFinder::GenerateV2(Double_t &phi, Double_t &eta, Double_t &pt) const
{
    phi = gRandom->Uniform(0, TMath::TwoPi());
    Double_t phi0(phi), v2(fImprintV2->Eval(pt)), f(0.), fp(0.), phiprev(0.);
    if(TMath::AreEqualAbs(v2, 0, 1e-5)) return;
    // introduce flow fluctuations (gaussian)
//    if(fFlowFluctuations > -10) GetFlowFluctuation(v2);
    for (Int_t i = 0; i < 100; i++) {
        phiprev = phi; //store last value for comparison
        f =  phi-phi0+v2*TMath::Sin(2.*(phi /*- fPsi2*/));
        fp = 1.0+2.0*v2*TMath::Cos(2.*(phi /*- fPsi2*/)); //first derivative
        phi -= f/fp;
        if (TMath::AreEqualAbs(phiprev, phi, 1e-10)) break; 
    }
}
//_____________________________________________________________________________
void AliGMFSimpleJetFinder::GenerateV3(Double_t &phi, Double_t &eta, Double_t &pt) const
{
    phi = gRandom->Uniform(0, TMath::TwoPi());
    Double_t phi0(phi), v3(fImprintV3->Eval(pt)), f(0.), fp(0.), phiprev(0.);
    if(TMath::AreEqualAbs(v3, 0, 1e-5) ) return;
    // introduce flow fluctuations (gaussian)
//    if(fFlowFluctuations > -10) GetFlowFluctuation(v3);
    for (Int_t i = 0; i < 100; i++)  {
        phiprev = phi; //store last value for comparison
        f =  phi-phi0+2./3.*v3*TMath::Sin(3.*(phi/*-fPsi3*/));
        fp = 1.0+2.0*v3*TMath::Cos(3.*(phi/*-fPsi3*/)); //first derivative
        phi -= f/fp;
        if (TMath::AreEqualAbs(phiprev, phi, 1e-10)) break;
    }
}
