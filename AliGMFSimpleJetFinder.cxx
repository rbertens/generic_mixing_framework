// simple jet finder on generic input vector
//
// Redmer Alexander Bertens, 2017 (UTK, CERN)

#include "fastjet/Selector.hh"
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
#include "TClonesArray.h"

#include "AliGMFEventContainer.h"
#include "AliGMFTTreeHeader.h"
#include "AliGMFTTreeTrack.h"
#include "AliGMFSimpleJetFinder.h"
#include "AliGMFHistogramManager.h"
#include "AliGMFSimpleEventCuts.h"

ClassImp(AliGMFSimpleJetFinder)

using namespace std;

//_____________________________________________________________________________
AliGMFSimpleJetFinder::AliGMFSimpleJetFinder() : TObject(),
    fEventNumber(0),
    fDoBackgroundSubtraction(kFALSE),
    fJetResolution(.3),
    fJetResolutionBkg(.3),
    fNCones(0),
    fLeadingHadronPt(.1),
    fLeadingHadronMaxPt(1e9),
    fRandomizeEtaPhi(kFALSE),
    fImprintV2(0x0),
    fImprintV3(0x0),
    fSplittingThreshold(1e9),
    fSplitTrackPt(3),
    fRandomizeSplitTrack(kTRUE),
    fPreserveSplitTrackPhi(kTRUE),
    fCollinearSplittingOverMEs(kFALSE),
    fIsME(kFALSE),
    fRejectNHardestJets(2),
    fPtAssLow(8),
    fPtAssHigh(9),
    fPtTrigLow(20),
    fPtTrigHigh(50),
    fSmearMean(0.),
    fSmearSigma(0.),
    fSmearRho(kFALSE),
    fEventCuts(0x0),
    fTrackCuts(0x0),
    fMockupEvent(0x0),
    fHistogramManager(0x0)

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
    fHistogramManager->BookTH1D("fHistConsideredConstituentPt", "p_{T}^{considered constituent}", 500, 0, 100);
    fHistogramManager->BookTH1D("fHistMockedUpConstituentPt", "p_{T}^{mocked up constituent}", 500, 0, 100);
    fHistogramManager->BookTH1D("fHistJetPtSubtracted", "p_{T}^{jet sub} = p_{T}^{jet} - #rho A ", 500, -130, 370); 
    fHistogramManager->BookTH2D("fHistRecoilJetBkgSpectrum", "p_{T}^{jet raw}", "#Delta #phi", 500, -130, 370, 40, 0, TMath::TwoPi()); 
    fHistogramManager->BookTH2D("fHistRecoilJetTriSpectrum", "p_{T}^{jet raw}", "#Delta #phi", 500, -130, 370, 40, 0, TMath::TwoPi()); 
    fHistogramManager->BookTH2D("fHistRecoilJetBkgSpectrumSub", "p_{T}^{jet sub} = p_{T}^{jet} - #rho A (bkg)", "#Delta #phi", 500, -130, 370, 40, 0, TMath::TwoPi()); 
    fHistogramManager->BookTH2D("fHistRecoilJetTriSpectrumSub", "p_{T}^{jet sub} = p_{T}^{jet} - #rho A (tri)", "#Delta #phi", 500, -130, 370, 40, 0, TMath::TwoPi()); 
    fHistogramManager->BookTH1D("fHistMultiplicity", "track multiplicity", 1000, 0, 4000);
    fHistogramManager->BookTH1D("fHistRho", "#rho", 100, 0, 250);
    fHistogramManager->BookTH2D("fHistMultiplicityRho", "track multiplicity", "#rho", 1000, 0, 4000, 100, 0, 250);
    fHistogramManager->BookTH2D("fHistCentralityRho", "centrality", "#rho", 100, 0, 100, 100, 0, 150);

    fHistogramManager->BookTH2D("fHistJetPtArea", "p_{T}^{jet}", "area", 100, 0, 100, 100, 0, 1);
    fHistogramManager->BookTH2D("fHistJetEtaPhi", "#eta^{jet}", "#phi^{jet}", 100, -1, 1, 100, 0, TMath::TwoPi());
    fHistogramManager->BookTH1D("fHistDeltaPt", "#Delta p_{T}", 500, -130, 370); 
    fHistogramManager->BookTH1D("fHistDeltaPtExLJ", "#Delta p_{T}", 500, -130, 370); 
    fHistogramManager->BookTH1D("fHistVertex", "cm", 100, -12, 12);
    fHistogramManager->BookTH1D("fHistCentrality", "percentile", 100, 0, 100);
    fHistogramManager->BookTH1D("fHistEventPlane", "#Psi", 100, -4, 4);
    fHistogramManager->BookTH1D("fHistJetFinderSettings", "flag", 12, -.5, 11.5);

    fNCones = TMath::CeilNint(((1.8-2.*fJetResolution)*TMath::TwoPi())/(TMath::Pi()*fJetResolution*fJetResolution));

    TH1D* settings = static_cast<TH1D*>(fHistogramManager->GetHistogram("fHistJetFinderSettings"));
    settings->GetXaxis()->SetBinLabel(1, "fJetResolution");
    settings->SetBinContent(1, fJetResolution);
    settings->GetXaxis()->SetBinLabel(2, "fLeadingHadronPt");
    settings->SetBinContent(2, fLeadingHadronPt);
    settings->GetXaxis()->SetBinLabel(3, "fSplittingThreshold");
    settings->SetBinContent(3, fSplittingThreshold);
    settings->GetXaxis()->SetBinLabel(4, "fSplitTrackPt");
    settings->SetBinContent(4, fSplitTrackPt);
    settings->GetXaxis()->SetBinLabel(5, "fRandomizeEtaPhi");
    settings->SetBinContent(5, (int)fRandomizeEtaPhi);
    settings->GetXaxis()->SetBinLabel(6, "fRandomizeSplitTrack");
    settings->SetBinContent(6, (int)fRandomizeSplitTrack);
    settings->GetXaxis()->SetBinLabel(7, "fNCones");
    settings->SetBinContent(7, (int)fNCones);
    settings->GetXaxis()->SetBinLabel(8, "fPtAssLow");
    settings->SetBinContent(7, fPtAssLow);
    settings->GetXaxis()->SetBinLabel(9, "fPtAssHigh");
    settings->SetBinContent(7, fPtAssHigh);
    settings->GetXaxis()->SetBinLabel(10, "fPtTrigLow");
    settings->SetBinContent(7, fPtTrigLow);
    settings->GetXaxis()->SetBinLabel(11, "fPtTrigHigh");
    settings->SetBinContent(7, fPtTrigHigh);

    // always make a minimal track cuts object
    if(!fTrackCuts) {
        fTrackCuts = new AliGMFSimpleTrackCuts();
    }
    if(fCollinearSplittingOverMEs) {
        // make a mockup event container that we will use
        // to store tracks for redistribution over multiplie MEs
        TClonesArray* trackBuffer = new TClonesArray("AliGMFTTreeTrack", 100);
        for(Int_t i(0); i < 100; i++) new((*trackBuffer)[i]) AliGMFTTreeTrack();
        fMockupEvent = new AliGMFEventContainer(
                new AliGMFTTreeHeader(),
                trackBuffer, 0);
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
    AliGMFTTreeTrack* mockUpTrack = new AliGMFTTreeTrack();

    // track kinematics
    Double_t px(0), py(0), pz(0);
    Int_t j(0);

    // only used when randomizing in eta, phi
    Double_t phi(0), eta(0), pt(0), splitTrackPt(0);
    TH1* fHistConstituentPt(fHistogramManager->GetHistogram("fHistConsideredConstituentPt"));


    if(fCollinearSplittingOverMEs && fEventNumber > 0) {
        AliGMFTTreeTrack* tempTrackA(0x0);
        AliGMFTTreeTrack* tempTrackB(0x0);
        fMockupEvent->ResetTrackIterator();
        for(Int_t i(0); i < fMockupEvent->GetNumberOfTracks()-1; i++) {
            // try to get two tracks and see if they are not split from the same track
            tempTrackA = fMockupEvent->GetNextTrack();
            // in case there is no buffer, just stop this procedure
            if(!tempTrackA) break;
            // this is safe: if next track is not found, tempTrack will be set to NULL
            tempTrackB = fMockupEvent->GetNextTrack();
            if(tempTrackA && (!tempTrackB)) {
                // in this case there is no next track to compare with, so it's fine to put it in
                AddProtoJetToCollection(fjInputVector, tempTrackA, j);
                fHistogramManager->Fill(
                        "fHistMockedUpConstituentPt", 
                        tempTrackA->GetPt()
                        );
                fHistogramManager->Fill(
                        "fHistMockedUpConstituentEtaPhi",
                        tempTrackA->GetEta(),
                        tempTrackA->GetPhi()
                        );
                // do the 'cleanup' for the mockup event
                tempTrackA->Reset();
                // no need in continuing when we are out of mockup tracks
                break;           
            } else if((tempTrackA && tempTrackB) && (tempTrackA->GetNumber() != tempTrackB->GetNumber())) {
                // iterator is now at the position for the next track
                // we use now the mockup track, reset it, push the buffer back by one and continue the loop
                AddProtoJetToCollection(fjInputVector, tempTrackA, j);
                fHistogramManager->Fill(
                        "fHistMockedUpConstituentPt", 
                        tempTrackA->GetPt()
                        );
                fHistogramManager->Fill(
                        "fHistMockedUpConstituentEtaPhi",
                        tempTrackA->GetEta(),
                        tempTrackA->GetPhi()
                        );
                // do the 'cleanup' for the mockup event
                tempTrackA->Reset();
                fMockupEvent->PushBackTrackIterator();
            } else {
                fMockupEvent->PushBackTrackIterator();

            }
        }
    }

    for(Int_t i(0); i < event->GetNumberOfTracks(); i++) {
        track = event->GetTrack(i);
        // check if the track satisfies the cuts
        if(!fTrackCuts->IsSelected(track)) continue;
        // decide how to deal with the track based on its pt
        if(track->GetPt() <= fSplittingThreshold) {
            // track is selected and pt smaller than threshold, use it directly in jet finding
            AddProtoJetToCollection(fjInputVector, track, j);
            fHistogramManager->Fill(
                    "fHistConsideredConstituentPt", 
                    track->GetPt()
                    );
        } else if ((track->GetPt() > fSplittingThreshold) && (fSplitTrackPt <= 0)) {
            // in this case, if a track has a pt larger than the max pt,
            // truncate it to this max value, and discard the rest of the pt
            px = fSplittingThreshold*TMath::Cos(track->GetPhi()); 
            py = fSplittingThreshold*TMath::Sin(track->GetPhi());  
            pz = fSplittingThreshold*TMath::SinH(track->GetEta()); 
            AddProtoJetToCollection(fjInputVector, px, py, pz, j);
            fHistogramManager->Fill(
                    "fHistMockedUpConstituentPt", 
                    fSplittingThreshold
                    );
        } else {
            // track pt is larger than the threshold, keep the pt that is 'truncated off'
            // what is split off gets distributed randomly either in eta or in eta and phi
            // or it is distributed collinearly over different MEs
            // first step: truncate the track pt, and add this truncated track pt to the pseudojet
            splitTrackPt = (fHistConstituentPt->GetEntries() > 0) ? fHistConstituentPt->GetRandom() : gRandom->Uniform(.15, fSplittingThreshold);
            track->SetPt(track->GetPt() - splitTrackPt);
            px = splitTrackPt*TMath::Cos(track->GetPhi()); 
            py = splitTrackPt*TMath::Sin(track->GetPhi());  
            pz = splitTrackPt*TMath::SinH(track->GetEta()); 
            AddProtoJetToCollection(fjInputVector, px, py, pz, j);
            fHistogramManager->Fill(
                    "fHistMockedUpConstituentPt", 
                    splitTrackPt
                    );
            // now, as long as the track pt is still too high, 'create'
            // new tracks which have fixed pt and add them as pseudojet
            splitTrackPt = (fHistConstituentPt->GetEntries() > 0) ? fHistConstituentPt->GetRandom() : gRandom->Uniform(.15, fSplittingThreshold);
            while(track->GetPt() >= splitTrackPt) {
                if(fRandomizeSplitTrack) {
                    if(!fPreserveSplitTrackPhi) {
                        phi = gRandom->Uniform(0, TMath::TwoPi());
                        eta = gRandom->Uniform(-.9, 9);
                        // imprint flow if this is supplied
                        if(fImprintV2) GenerateV2(phi, pt);
                        else if(fImprintV3) GenerateV3(phi, pt);
                    } else if(fPreserveSplitTrackPhi) {
                        // keep the phi angle intact, but embed the new track far
                        // away enough from the initial jet to not introduce a bias
                        if(track->GetEta() > 0.) eta = gRandom->Uniform(-.9, -.2);
                        else eta = gRandom->Uniform(.2, .9);
                        phi = track->GetPhi();
                    }
                    px = splitTrackPt*TMath::Cos(phi);
                    py = splitTrackPt*TMath::Sin(phi); 
                    pz = splitTrackPt*TMath::SinH(eta); 
                } else {
                    px = splitTrackPt*TMath::Cos(track->GetPhi()); 
                    py = splitTrackPt*TMath::Sin(track->GetPhi());  
                    pz = splitTrackPt*TMath::SinH(track->GetEta()); 
                }
                // depending on whether or not collinear redistribution over MEs is requested
                // choose to plug in the track here, or buffer it for later
                if(fCollinearSplittingOverMEs) {
                    mockUpTrack->Reset();
                    mockUpTrack->Fill(splitTrackPt, track->GetEta(), GetPhi(), track->GetCharge(), kTRUE, Concatenate(fEventNumber,i));
                    fMockupEvent->FindOrCreateEmptyTrack()->Fill(mockUpTrack);
                } else {
                    AddProtoJetToCollection(fjInputVector, px, py, pz, j);
                    fHistogramManager->Fill(
                            "fHistMockedUpConstituentPt", 
                            splitTrackPt
                            );
                }
                // decrease the track pt here: the result of the truncation
                track->SetPt(track->GetPt() - splitTrackPt);
                splitTrackPt = (fHistConstituentPt->GetEntries() > 0) ? fHistConstituentPt->GetRandom() : gRandom->Uniform(.15, fSplittingThreshold);
            } 
            // then we just add the remaining pt as track
            pt = track->GetPt();
            if(fRandomizeSplitTrack) {
                if(!fPreserveSplitTrackPhi) {
                    phi = gRandom->Uniform(0, TMath::TwoPi());
                    eta = gRandom->Uniform(-.9, 9);
                    // imprint flow if this is supplied
                    if(fImprintV2) GenerateV2(phi, pt);
                    else if(fImprintV3) GenerateV3(phi, pt);
                } else if(fPreserveSplitTrackPhi) {
                    // keep the phi angle intact, but embed the new track far
                    // away enough from the initial jet to not introduce a bias
                    if(track->GetEta() > 0.) eta = gRandom->Uniform(-.9, -.2);
                    else eta = gRandom->Uniform(.2, .9);
                    phi = track->GetPhi();
                }
                px = pt*TMath::Cos(phi);
                py = pt*TMath::Sin(phi); 
                pz = pt*TMath::SinH(eta); 
            } else {
                px = pt*TMath::Cos(track->GetPhi()); 
                py = pt*TMath::Sin(track->GetPhi());  
                pz = pt*TMath::SinH(track->GetEta()); 
            }
            if(fCollinearSplittingOverMEs) {
                mockUpTrack->Reset();
                mockUpTrack->Fill(splitTrackPt, track->GetEta(), track->GetPhi(), track->GetCharge(), kTRUE, Concatenate(fEventNumber, i));
                fMockupEvent->FindOrCreateEmptyTrack()->Fill(mockUpTrack);
            } else {
                AddProtoJetToCollection(fjInputVector, px, py, pz, j);
                fHistogramManager->Fill(
                        "fHistMockedUpConstituentPt", 
                        pt
                        );
            }
        }
    }
    fHistogramManager->Fill("fHistMultiplicity", j);

    if(fCollinearSplittingOverMEs && fEventNumber < 5) {
        fEventNumber++;
        return kTRUE;
    }


    // setup the jet finder for signal and background jets
    fastjet::GhostedAreaSpec     ghostSpec(.95, 1, 0.001, 1, .1, 1e-100);
    fastjet::Strategy            strategy = fastjet::Best;
    fastjet::RecombinationScheme recombScheme = fastjet::BIpt_scheme;
    fastjet::AreaType            areaType = fastjet::active_area;
    fastjet::AreaDefinition      areaDef = fastjet::AreaDefinition(areaType, ghostSpec);

    // some bookkeeping
    // FIXME change range definition to selector 
    //    fastjet::Selector range(fastjet::SelectorAbsRapMax(1.-.95*fJetResolution));
    fastjet::RangeDefinition range(fJetResolution-.9, .9-fJetResolution, 0, 2.*fastjet::pi);
    fastjet::RangeDefinition rangeRho(-fJetResolutionBkg-.9, .9-fJetResolutionBkg, 0, 2.*fastjet::pi);
    fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, fJetResolution, recombScheme, strategy);
    fastjet::JetDefinition jetDefRho(fastjet::kt_algorithm, fJetResolutionBkg, recombScheme, strategy);

    // feed the protojets to fastjet
    fastjet::ClusterSequenceArea clusterSeq(fjInputVector, jetDef, areaDef);
    fastjet::ClusterSequenceArea clusterSeqRho(fjInputVector, jetDefRho, areaDef);

    // get the jets
    std::vector <fastjet::PseudoJet> inclusiveJets = clusterSeq.inclusive_jets();
    std::vector <fastjet::PseudoJet> backgroundJets = clusterSeqRho.inclusive_jets();


    // vector for the background energy density
    Double_t rhoVector[backgroundJets.size()];
    Int_t iBGJets(0);
    // helper vectors for the bubble sorter - array sizes are maxima, not all indices are used
    Double_t ptVector[backgroundJets.size()];
    Double_t trimmedRhoVector[backgroundJets.size()-fRejectNHardestJets];

    // first, store background energy density per jet and jet pt of all jets in acceptance
    for (UInt_t iJet = 0; iJet < backgroundJets.size(); iJet++) {
        if (rangeRho.is_in_range(backgroundJets[iJet]) && backgroundJets[iJet].area() > 0.01) {
            rhoVector[iBGJets] = backgroundJets[iJet].perp() / backgroundJets[iJet].area();
            ptVector[iBGJets] = backgroundJets[iJet].perp();
            iBGJets++;
        }
    }

    // sort the jets in pt to identify the hardest jets in the background
    if(fRejectNHardestJets > 0) {
        // this array will contain the jet indices, sorted descending in energy
        Bool_t flippedIndex(kTRUE);
        Double_t temp(0);
        while (flippedIndex) {
            // no flip has been performed yet
            flippedIndex = kFALSE;
            for (UInt_t iJet = 0; iJet < (UInt_t)(iBGJets - 1); iJet++) {
                // get the i-th and i-th+1 jet and compare their pt
                // check if pt of jet i+1 is larger than pt of jet i
                if(ptVector[iJet + 1] > ptVector[iJet]) {
                    // and flip pt
                    temp = ptVector[iJet];
                    ptVector[iJet] = ptVector[iJet + 1];
                    ptVector[iJet + 1] = temp;
                    // flip rho 
                    temp = rhoVector[iJet];
                    rhoVector[iJet] = rhoVector[iJet + 1];
                    rhoVector[iJet + 1] = temp;
                    // tell sorter that we changed the order in this pass
                    flippedIndex = kTRUE;
                }
            }
        }
    }

    Double_t rho(0);
    if(fRejectNHardestJets == 0) {
        rho = TMath::Median(iBGJets, rhoVector);
    } else {
        Int_t trimmedI(0);
        for (UInt_t iJet = fRejectNHardestJets; iJet < (UInt_t)iBGJets; iJet++) {
            trimmedRhoVector[trimmedI] = rhoVector[iJet];
            trimmedI++;
        }
        // note: math will try to sort the vector, but since it's already sorted, this is a trivial op
        rho = TMath::Median(trimmedI, trimmedRhoVector);
    }

    if(fSmearRho) {
        rho += gRandom->Gaus(fSmearMean, fSmearSigma);
    }

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

    /*
       Float_t rcPt(0), rcEta(0), rcPhi(0);
       for(Int_t i = 0; i < fNCones; i++) {
       Bool_t unbiased = GetRandomCone(event, rcPt, rcEta, rcPhi, 
       backgroundJets[0].eta(), backgroundJets[0].phi());
       if(unbiased) fHistogramManager->Fill(
       "fHistDeltaPtExLJ",
       rcPt - rho*TMath::Pi()*fJetResolution*fJetResolution
       );
       fHistogramManager->Fill(
       "fHistDeltaPt",
       rcPt - rho*TMath::Pi()*fJetResolution*fJetResolution
       );
       }
       */      

    // fill the jet histograms
    for (UInt_t iJet = 0; iJet < inclusiveJets.size(); iJet++) {
        if (!range.is_in_range(inclusiveJets[iJet])) continue;
        // loop over constituents to apply leading hadron cut
        std::vector<fastjet::PseudoJet> constituents = clusterSeq.constituents(inclusiveJets[iJet]);
        Double_t maxpt(0), pickMe(0);
        for(UInt_t i(0); i < constituents.size(); i++) {
            if(constituents[i].perp() > maxpt) maxpt = constituents[i].perp();
        }
        // check jet area and constituent requirement
        if(maxpt < fLeadingHadronPt || maxpt > fLeadingHadronMaxPt) continue;
        if(inclusiveJets[iJet].area() < .56*TMath::Pi()*fJetResolution*fJetResolution) continue;

        // unfortunately tracks need to be filled in a second loop over the constituents 
        // by looping over the constituents *within* the jet loop, we de-factor loop over all
        // tracks, taking into account the possible track splitting
        for(UInt_t i(0); i < constituents.size(); i++) {
            fHistogramManager->Fill(
                    "fHistConstituentPt", 
                    constituents[i].perp()
                    );
            // do recoil jet yield analysis in one go 
            if(fIsME) {
                // just pick a random track every so often, and then bookkeep the angular distribution
                // of jets w.r.t. this track
                pickMe = gRandom->Uniform(0,1);
                if(pickMe > .99) {
                    for (UInt_t jJet = 0; jJet < inclusiveJets.size(); jJet++) {
                        if (!range.is_in_range(inclusiveJets[jJet])) continue;
                        fHistogramManager->Fill(
                                "fHistRecoilJetBkgSpectrum",
                                inclusiveJets[jJet].perp(),
                                PhaseShift(TMath::Pi() - (inclusiveJets[jJet].phi() - constituents[i].phi())));
                        fHistogramManager->Fill(
                                "fHistRecoilJetBkgSpectrumSub",
                                inclusiveJets[jJet].perp() - rho * inclusiveJets[jJet].area(),
                                PhaseShift(TMath::Pi() - (inclusiveJets[jJet].phi() - constituents[i].phi())));
                    }
                } else if (pickMe < .01) {
                    for (UInt_t jJet = 0; jJet < inclusiveJets.size(); jJet++) {
                        if (!range.is_in_range(inclusiveJets[jJet])) continue;
                        fHistogramManager->Fill(
                                "fHistRecoilJetTriSpectrum",
                                inclusiveJets[jJet].perp(),
                                PhaseShift(TMath::Pi() - (inclusiveJets[jJet].phi() - constituents[i].phi())));
                        fHistogramManager->Fill(
                                "fHistRecoilJetTriSpectrumSub",
                                inclusiveJets[jJet].perp() - rho * inclusiveJets[jJet].area(),
                                PhaseShift(TMath::Pi() - (inclusiveJets[jJet].phi() - constituents[i].phi())));
                    }
                }
            } else {
                if(constituents[i].perp() > fPtAssLow && constituents[i].perp() < fPtAssHigh) {
                    for (UInt_t jJet = 0; jJet < inclusiveJets.size(); jJet++) {
                        if (!range.is_in_range(inclusiveJets[jJet])) continue;
                        fHistogramManager->Fill(
                                "fHistRecoilJetBkgSpectrum",
                                inclusiveJets[jJet].perp(),
                                PhaseShift(TMath::Pi() - (inclusiveJets[jJet].phi() - constituents[i].phi())));
                        fHistogramManager->Fill(
                                "fHistRecoilJetBkgSpectrumSub",
                                inclusiveJets[jJet].perp() - rho * inclusiveJets[jJet].area(),
                                PhaseShift(TMath::Pi() - (inclusiveJets[jJet].phi() - constituents[i].phi())));
                    }
                } else if (constituents[i].perp() > fPtTrigLow && constituents[i].perp() < fPtTrigHigh) {
                    for (UInt_t jJet = 0; jJet < inclusiveJets.size(); jJet++) {
                        if (!range.is_in_range(inclusiveJets[jJet])) continue;
                        fHistogramManager->Fill(
                                "fHistRecoilJetTriSpectrum",
                                inclusiveJets[jJet].perp(),
                                PhaseShift(TMath::Pi() - (inclusiveJets[jJet].phi() - constituents[i].phi())));
                        fHistogramManager->Fill(
                                "fHistRecoilJetTriSpectrumSub",
                                inclusiveJets[jJet].perp() - rho * inclusiveJets[jJet].area(),
                                PhaseShift(TMath::Pi() - (inclusiveJets[jJet].phi() - constituents[i].phi())));
                    }
                }
            }
        }
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

    fEventNumber++;
    return kTRUE;
}
//_____________________________________________________________________________
void AliGMFSimpleJetFinder::ApplyEventWeight(Double_t weight) {
    // apply an event weight
    fHistogramManager->WeighAllHistograms(weight);
}
//_____________________________________________________________________________
Bool_t AliGMFSimpleJetFinder::Finalize(TFile* of) {
    of->cd();
    fHistogramManager->StoreManager(of);
    return kTRUE;
}
//_____________________________________________________________________________
void AliGMFSimpleJetFinder::GenerateV2(Double_t &phi, Double_t &pt) const
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
void AliGMFSimpleJetFinder::GenerateV3(Double_t &phi, Double_t &pt) const
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
//_____________________________________________________________________________
Bool_t AliGMFSimpleJetFinder::GetRandomCone(AliGMFEventContainer* event, Float_t &pt, Float_t &eta, Float_t &phi, Float_t etaJet, Float_t phiJet)
{
    // get a random cone, returns false if cone overlaps with provided jet
    pt = 0; eta = 0; phi = 0;
    eta = gRandom->Uniform(-.9+fJetResolution, .9-fJetResolution);
    phi = gRandom->Uniform(0, TMath::TwoPi());

    for(Int_t i(0); i < event->GetNumberOfTracks(); i++) {
        AliGMFTTreeTrack* track = event->GetTrack(i);
        if(fTrackCuts->IsSelected(track)) {
            Float_t etaTrack(track->GetEta()), phiTrack(track->GetPhi());
            // get distance from cone
            if(TMath::Abs(phiTrack-phi) > TMath::Abs(phiTrack - phi + TMath::TwoPi())) phiTrack+=TMath::TwoPi();
            if(TMath::Abs(phiTrack-phi) > TMath::Abs(phiTrack - phi - TMath::TwoPi())) phiTrack-=TMath::TwoPi();
            if(TMath::Sqrt(TMath::Abs((etaTrack-eta)*(etaTrack-eta)+(phiTrack-phi)*(phiTrack-phi))) < fJetResolution) pt += track->GetPt();
        }
    }
    // check if cone overlaps with provided 
    return (TMath::Sqrt((etaJet-eta)*(etaJet-eta)+(phiJet-phi)*(phiJet-phi)) > fJetResolution);
}
//_____________________________________________________________________________
void     AliGMFSimpleJetFinder::AddProtoJetToCollection( std::vector<fastjet::PseudoJet>& protoJetCollection,
       Double_t &px, Double_t &py, Double_t &pz, Int_t &j ) {
    // add a protojet to a protojet collection
    if(fCollinearSplittingOverMEs && fEventNumber < 5) return;
    Double_t totalE = px*px+py*py+pz*pz;
    if (!(totalE >  0)) return;
    fastjet::PseudoJet fjInputProtoJet(
            px, 
            py, 
            pz, 
            TMath::Sqrt(totalE));
    fjInputProtoJet.set_user_index(j);
    protoJetCollection.push_back(fjInputProtoJet);
    j++;
}
//_____________________________________________________________________________
void     AliGMFSimpleJetFinder::AddProtoJetToCollection( std::vector<fastjet::PseudoJet>& protoJetCollection, AliGMFTTreeTrack* track, Int_t &j ) {
    // add a protojet to a protojet collection
    if(fCollinearSplittingOverMEs && fEventNumber < 5) return;
    Double_t px = track->GetPt()*TMath::Cos(track->GetPhi()); 
    Double_t py = track->GetPt()*TMath::Sin(track->GetPhi());  
    Double_t pz = track->GetPt()*TMath::SinH(track->GetEta()); 

    AddProtoJetToCollection(protoJetCollection, px, py, pz, j);
}
